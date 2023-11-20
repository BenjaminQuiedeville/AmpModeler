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
    
    upSampleFactor = 2
    overSampler = new juce::dsp::Oversampling<sample_t>(
        1, upSampleFactor, 
        juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR
    );
}

PreampDistorsion::~PreampDistorsion() {
    delete overSampler;
}

void PreampDistorsion::prepareToPlay(double _samplerate, int blockSize) {
    samplerate = _samplerate;

    preGain.init(samplerate, 0.02, 0.0, SMOOTH_PARAM_LIN);
    postGain.init(samplerate, 0.02, 0.0, SMOOTH_PARAM_LIN);

    inputFilter->prepareToPlay(samplerate*upSampleFactor);
    inputFilter->setCoefficients(inputFilterFrequency);

    overSampler->initProcessing(blockSize);

    preGain.newTarget(DB_TO_GAIN(0.0));
    postGain.newTarget(DB_TO_GAIN(-12.0));

    stageGain = DB_TO_GAIN(20.0);
    stageAttenuation = 0.5;
}

static inline sample_t waveShaping(sample_t sample) {
    
    sample *= -1.0f;

    if (sample > 0.0f) { sample = std::tanh(sample); }
    if (sample < -1.0f) { sample = -1.0f; }

    return sample;
}


void PreampDistorsion::process(float *input, size_t nSamples) {
        
    AudioBlock upSampledBlock = 
        overSampler->processSamplesUp(AudioBlock(&input, 1, nSamples));

    inputFilter->processBufferHighpass(input, nSamples);

    sample_t *upSampledData = upSampledBlock.getChannelPointer(0);

    for (size_t index = 0; index < nSamples*(overSampler->getOversamplingFactor()); index++) {
        
        sample_t sample = upSampledData[index];

        sample *= stageGain;
        sample = waveShaping(sample);
        sample *= 0.5;

        sample = preGain.nextValue() * sample;

        sample *= stageGain;
        sample = waveShaping(sample);
        sample *= 0.5;

        sample *= stageGain;
        sample = waveShaping(sample);
        sample *= 0.5;

        sample *= stageGain;
        sample = waveShaping(sample);
        sample *= 0.5;
        
        sample = postGain.nextValue() * sample;
    }

    auto outputBlock = AudioBlock(&input, 1, nSamples);

    overSampler->processSamplesDown(outputBlock);

    memcpy((void *)input, (void *)outputBlock.getChannelPointer(0), nSamples);

}