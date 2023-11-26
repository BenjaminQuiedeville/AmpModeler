/*
  ==============================================================================

    PreampDistorsion.cpp
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreampDistorsion.h"
#include <memory>

PreampDistorsion::PreampDistorsion() {
    
    upSampleFactor = 2;
    overSampler = new juce::dsp::Oversampling<sample_t>(
        1, upSampleFactor, 
        juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR
    );

    preGain  = new SmoothParam();
    postGain = new SmoothParam();

    inputFilter = new OnepoleFilter();

    couplingFilter1 = new OnepoleFilter();
    couplingFilter2 = new OnepoleFilter();
    couplingFilter3 = new OnepoleFilter();

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

    delete tubeBypassFilter1;
    delete tubeBypassFilter2;
}

void PreampDistorsion::prepareToPlay(double _samplerate, int blockSize) {
    samplerate = _samplerate;

    preGain->init(samplerate, 0.02, 0.0, SMOOTH_PARAM_LIN);
    postGain->init(samplerate, 0.02, 0.0, SMOOTH_PARAM_LIN);

    inputFilter->prepareToPlay(samplerate*upSampleFactor);
    couplingFilter1->prepareToPlay(samplerate*upSampleFactor);
    couplingFilter2->prepareToPlay(samplerate*upSampleFactor);
    couplingFilter3->prepareToPlay(samplerate*upSampleFactor);
    tubeBypassFilter1->prepareToPlay(samplerate*upSampleFactor);
    tubeBypassFilter2->prepareToPlay(samplerate*upSampleFactor);

    inputFilter->setCoefficients(40.0);
    couplingFilter1->setCoefficients(50.0);
    couplingFilter2->setCoefficients(50.0);
    couplingFilter3->setCoefficients(50.0);

    tubeBypassFilter1->setCoefficients(100.0, 0.6, 6.0);
    tubeBypassFilter2->setCoefficients(100.0, 0.6, 6.0);

    overSampler->initProcessing(blockSize);

    preGain->newTarget(DB_TO_GAIN(0.0));
    postGain->newTarget(DB_TO_GAIN(-12.0));

    stageGain = DB_TO_GAIN(25.0);
}

static inline sample_t waveShaping(sample_t sample) {
    
    sample *= -1.0f;

    if (sample > 0.0f) { sample = std::tanh(sample); }
    if (sample < -1.0f) { sample = -1.0f; }

    return sample;
}


void PreampDistorsion::process(float *buffer, size_t nSamples) {
        
    AudioBlock upSampledBlock = 
        overSampler->processSamplesUp(AudioBlock(&buffer, 1, nSamples));

    inputFilter->processBufferHighpass(buffer, nSamples);

    sample_t *upSampledData = upSampledBlock.getChannelPointer(0);

    for (size_t index = 0; index < nSamples*(overSampler->getOversamplingFactor()); index++) {
        
        sample_t sample = upSampledData[index];

        // input Tube stage

        sample = tubeBypassFilter1->processSample(sample);
        sample *= stageGain;
        sample = waveShaping(sample);
        
        sample = inputFilter->processHighPass(sample);
        sample *= 0.9f;

        sample = preGain->nextValue() * sample;

        // Second Tube first stage of saturation, stop there for clean tone

        sample *= stageGain;
        sample = waveShaping(sample);
        sample *= 0.5f;
        sample = couplingFilter1->processHighPass(sample);


        // Third tube stage
        sample = tubeBypassFilter2->processSample(sample);
        sample *= stageGain;
        sample = waveShaping(sample);
        sample *= 0.5f;
        sample = couplingFilter2->processHighPass(sample);

        // Fourth tube stage
        sample *= stageGain;
        sample = waveShaping(sample);
        sample *= 0.5f;
        sample = couplingFilter3->processHighPass(sample);

        sample *= outputAttenuation;
        
        sample = postGain->nextValue() * sample;
    }

    auto outputBlock = AudioBlock(&buffer, 1, nSamples);

    overSampler->processSamplesDown(outputBlock);

    memcpy((void *)buffer, (void *)outputBlock.getChannelPointer(0), nSamples);

}