/*
  ==============================================================================

    PreampDistorsion.cpp
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#include "Preamp.h"
#include <memory>
#include <assert.h>

OverSampler::OverSampler() {
    upSampleFilter1 = new Biquad(BIQUAD_LOWPASS);
    upSampleFilter2 = new Biquad(BIQUAD_LOWPASS);

    downSampleFilter1 = new Biquad(BIQUAD_LOWPASS);
    downSampleFilter2 = new Biquad(BIQUAD_LOWPASS);
}

OverSampler::~OverSampler() {
    delete upSampleFilter1;
    delete upSampleFilter2;

    delete downSampleFilter1;
    delete downSampleFilter2;
}

void OverSampler::prepareToPlay(double _samplerate) {

    upSampleFilter1->prepareToPlay();
    upSampleFilter2->prepareToPlay();
    downSampleFilter1->prepareToPlay();
    downSampleFilter2->prepareToPlay();

    upSampleFilter1->setCoefficients(_samplerate/2 * 0.8, 0.7, GAIN_TO_DB(upSampleFactor), _samplerate*upSampleFactor);
    upSampleFilter2->setCoefficients(_samplerate/2 * 0.8, 0.7, GAIN_TO_DB(upSampleFactor), _samplerate*upSampleFactor);
    downSampleFilter1->setCoefficients(_samplerate/2 * 0.8, 0.7, 0.0, _samplerate*upSampleFactor);
    downSampleFilter2->setCoefficients(_samplerate/2 * 0.8, 0.7, 0.0, _samplerate*upSampleFactor);

}

void OverSampler::upSample(sample_t *source, sample_t *upSampled, size_t sourceSize, size_t upSampledSize) {

    assert(upSampledSize == sourceSize * upSampleFactor);

    memset(upSampled, 0, upSampledSize * sizeof(sample_t));

    for (size_t i = 0; i < sourceSize; i++) {
        upSampled[upSampleFactor*i] = source[i];
    }

    upSampleFilter1->processBuffer(upSampled, upSampledSize);    
    upSampleFilter2->processBuffer(upSampled, upSampledSize);    

}

void OverSampler::downSample(sample_t *upSampled, sample_t *dest, size_t upSampledSize, size_t destSize) {

    assert(upSampledSize == destSize*upSampleFactor);

    downSampleFilter1->processBuffer(upSampled, upSampledSize);
    downSampleFilter2->processBuffer(upSampled, upSampledSize);

    for (size_t i = 0; i < destSize; i++) {
        dest[i] = upSampled[i*upSampleFactor];
    }
}


PreampDistorsion::PreampDistorsion() {
    
    upSampleFactor = 4;
    overSampler = new juce::dsp::Oversampling<sample_t>(
        1, upSampleFactor, 
        juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR
    );

    preGain  = new SmoothParam();
    postGain = new SmoothParam();

    inputFilter     = new OnepoleFilter();
    couplingFilter1 = new OnepoleFilter();
    couplingFilter2 = new OnepoleFilter();
    couplingFilter3 = new OnepoleFilter();

    stageOutputFilter1 = new OnepoleFilter();
    stageOutputFilter2 = new OnepoleFilter();

    tubeBypassFilter1 = new Biquad(BIQUAD_HIGHSHELF);
    tubeBypassFilter2 = new Biquad(BIQUAD_HIGHSHELF);
}

PreampDistorsion::~PreampDistorsion() {
    delete overSampler;

    delete preGain;
    delete postGain;

    delete inputFilter;
    delete couplingFilter1;
    delete couplingFilter2;
    delete couplingFilter3;

    delete stageOutputFilter1;
    delete stageOutputFilter2;

    delete tubeBypassFilter1;
    delete tubeBypassFilter2;
}

void PreampDistorsion::prepareToPlay(double _samplerate, int blockSize) {
    samplerate = _samplerate;

    preGain->init(samplerate, 0.02, DB_TO_GAIN(0.0), SMOOTH_PARAM_LIN);
    postGain->init(samplerate, 0.02, DB_TO_GAIN(-12.0), SMOOTH_PARAM_LIN);
    
    inputFilter->prepareToPlay();
    couplingFilter1->prepareToPlay();
    couplingFilter2->prepareToPlay();
    couplingFilter3->prepareToPlay();

    inputFilter->setCoefficients(40.0, samplerate*upSampleFactor);
    couplingFilter1->setCoefficients(50.0, samplerate*upSampleFactor);
    couplingFilter2->setCoefficients(50.0, samplerate*upSampleFactor);
    couplingFilter3->setCoefficients(50.0, samplerate*upSampleFactor);

    stageOutputFilter1->setCoefficients(2000.0, samplerate*upSampleFactor);
    stageOutputFilter2->setCoefficients(2000.0, samplerate*upSampleFactor);

    tubeBypassFilter1->setCoefficients(100.0, 0.6, 6.0, samplerate*upSampleFactor);
    tubeBypassFilter2->setCoefficients(100.0, 0.6, 6.0, samplerate*upSampleFactor);

    overSampler->initProcessing(blockSize);

    stageGain = (float)DB_TO_GAIN(25.0);
}

static inline sample_t waveShaping(sample_t sample, float headroom) {
    
    sample = -sample / headroom;

    if (sample > 0.0f) { sample = std::tanh(sample); }
    if (sample < -1.0f) { sample = -1.0f; }

    return sample*headroom;
}


void PreampDistorsion::process(float *buffer, size_t nSamples) {
        
    AudioBlock upSampledBlock = 
        overSampler->processSamplesUp(AudioBlock(&buffer, 1, nSamples));

    sample_t *upSampledData = upSampledBlock.getChannelPointer(0);

    for (size_t index = 0; index < nSamples*(overSampler->getOversamplingFactor()); index++) {
        
        sample_t sample = upSampledData[index];

        // input Tube stage

        sample = tubeBypassFilter1->process(sample);
        sample *= stageGain;
        sample = waveShaping(sample, 2*headroom);
        
        sample = inputFilter->processHighPass(sample);
        sample *= 0.9f;

        sample = (sample_t)DB_TO_GAIN(preGain->nextValue()) * sample;


        // Second Tube first stage of saturation, stop there for clean tone
        sample *= stageGain;
        sample = waveShaping(sample, headroom);
        sample *= 0.5f;
        sample = couplingFilter1->processHighPass(sample);


        // Third tube stage
        sample = tubeBypassFilter2->process(sample);
        sample *= stageGain;
        sample = waveShaping(sample, headroom);
        sample *= 0.5f;
        sample = couplingFilter2->processHighPass(sample);
        sample = stageOutputFilter1->processLowPass(sample);


        // Fourth tube stage
        sample *= stageGain;
        sample = waveShaping(sample, headroom);
        // sample *= 0.5f;
        sample = couplingFilter3->processHighPass(sample);
        sample = stageOutputFilter2->processLowPass(sample);

        sample *= outputAttenuation;
        
        sample *= (sample_t)DB_TO_GAIN(postGain->nextValue());
        sample /= headroom;

        upSampledData[index] = sample;
    }

    auto outputBlock = AudioBlock(&buffer, 1, nSamples);

    overSampler->processSamplesDown(outputBlock);

    memcpy((void *)buffer, (void *)outputBlock.getChannelPointer(0), nSamples);

}