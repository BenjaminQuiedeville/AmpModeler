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


void OverSampler::prepareToPlay(double _samplerate) {

    upSampleFilter1.prepareToPlay();
    upSampleFilter2.prepareToPlay();
    downSampleFilter1.prepareToPlay();
    downSampleFilter2.prepareToPlay();

    upSampleFilter1.setCoefficients(_samplerate/2 * 0.9, 0.7, GAIN_TO_DB(PREAMP_UP_SAMPLE_FACTOR), _samplerate*PREAMP_UP_SAMPLE_FACTOR);
    upSampleFilter2.setCoefficients(_samplerate/2 * 0.9, 0.7, GAIN_TO_DB(PREAMP_UP_SAMPLE_FACTOR), _samplerate*PREAMP_UP_SAMPLE_FACTOR);
    downSampleFilter1.setCoefficients(_samplerate/2 * 0.9, 0.7, 0.0, _samplerate*PREAMP_UP_SAMPLE_FACTOR);
    downSampleFilter2.setCoefficients(_samplerate/2 * 0.9, 0.7, 0.0, _samplerate*PREAMP_UP_SAMPLE_FACTOR);

}

void OverSampler::upSample(sample_t *source, sample_t *upSampled, size_t sourceSize, size_t upSampledSize) {

    assert(upSampledSize == sourceSize * PREAMP_UP_SAMPLE_FACTOR);

    memset(upSampled, 0, upSampledSize * sizeof(sample_t));

    for (size_t i = 0; i < sourceSize; i++) {
        upSampled[PREAMP_UP_SAMPLE_FACTOR*i] = source[i];
    }

    upSampleFilter1.processBuffer(upSampled, upSampledSize);    
    upSampleFilter2.processBuffer(upSampled, upSampledSize);    

}

void OverSampler::downSample(sample_t *upSampled, sample_t *dest, size_t upSampledSize, size_t destSize) {

    assert(upSampledSize == destSize*PREAMP_UP_SAMPLE_FACTOR);

    downSampleFilter1.processBuffer(upSampled, upSampledSize);
    downSampleFilter2.processBuffer(upSampled, upSampledSize);

    for (size_t i = 0; i < destSize; i++) {
        dest[i] = upSampled[i*PREAMP_UP_SAMPLE_FACTOR];
    }
}


PreampDistorsion::PreampDistorsion() {
    
    overSampler = new OverSampler();
}

PreampDistorsion::~PreampDistorsion() {
    delete overSampler;

    if (upSampledBlock) { free(upSampledBlock); }

}

void PreampDistorsion::prepareToPlay(double _samplerate, int blockSize) {
    samplerate = _samplerate;

    preGain.init(DB_TO_GAIN(0.0));
    postGain.init(DB_TO_GAIN(-12.0));
    
    inputFilter.prepareToPlay();
    couplingFilter1.prepareToPlay();
    couplingFilter2.prepareToPlay();
    couplingFilter3.prepareToPlay();
    
    stageOutputFilter1.prepareToPlay();
    stageOutputFilter2.prepareToPlay();
    

    inputFilter.setCoefficients(100.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter1.setCoefficients(20.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter2.setCoefficients(20.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter3.setCoefficients(20.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    stageOutputFilter1.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter2.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter3.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    tubeBypassFilter1.setCoefficients(200.0, 0.4, 6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    tubeBypassFilter2.setCoefficients(200.0, 0.4, 6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    overSampler->prepareToPlay(_samplerate);

    if (upSampledBlock) {
        upSampledBlock = (sample_t *)realloc(upSampledBlock, blockSize * PREAMP_UP_SAMPLE_FACTOR * sizeof(sample_t));
    } else {
        upSampledBlock = (sample_t *)calloc(blockSize * PREAMP_UP_SAMPLE_FACTOR, sizeof(sample_t));
    }

}

static inline sample_t waveShaping(sample_t sample, float headroom) {
    
    sample = -sample / headroom;

    if (sample > 0.0f) { sample = std::tanh(sample); }
    if (sample < -1.0f) { sample = -1.0f; }

    return sample*headroom;
}


void PreampDistorsion::process(float *buffer, size_t nSamples) {
        

    overSampler->upSample(buffer, upSampledBlock, nSamples, nSamples*PREAMP_UP_SAMPLE_FACTOR);

    for (size_t index = 0; index < nSamples*PREAMP_UP_SAMPLE_FACTOR; index++) {
        
        sample_t sample = upSampledBlock[index];

        // input Tube stage

        sample = tubeBypassFilter1.process(sample);
        sample *= STAGE_GAIN;
        sample = waveShaping(sample, 5*headroom);
        
        sample = inputFilter.processHighPass(sample);
        sample = stageOutputFilter1.processLowPass(sample);
        sample *= 0.9f;

        sample *= (sample_t)preGain.nextValue();


        // Second Tube first stage of saturation, stop there for clean tone
        sample *= STAGE_GAIN;
        sample = waveShaping(sample, headroom);
        sample *= 0.5f;
        sample = couplingFilter1.processHighPass(sample);


        // Third tube stage
        sample = tubeBypassFilter2.process(sample);
        sample *= STAGE_GAIN;
        sample = waveShaping(sample, headroom);
        sample *= 0.5f;
        sample = couplingFilter2.processHighPass(sample);
        sample = stageOutputFilter2.processLowPass(sample);


        // Fourth tube stage
        sample *= STAGE_GAIN;
        sample = waveShaping(sample, headroom);
        // sample *= 0.5f;
        sample = couplingFilter3.processHighPass(sample);
        sample = stageOutputFilter3.processLowPass(sample);

        sample *= OUTPUT_ATTENUATION;
        
        sample *= (sample_t)DB_TO_GAIN(postGain.nextValue());
        sample /= headroom;

        upSampledBlock[index] = sample;
    }

    
    overSampler->downSample(upSampledBlock, buffer, nSamples * PREAMP_UP_SAMPLE_FACTOR, nSamples);

}