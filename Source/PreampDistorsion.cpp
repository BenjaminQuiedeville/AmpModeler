/*
  ==============================================================================

    PreampDistorsion.cpp
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreampDistorsion.h"

PreampDistorsion::PreampDistorsion() {
    
    overSampler = std::make_unique<juce::dsp::Oversampling<sample_t>>(
        1, 1, 
        juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR
    );

    driveType = CUBIC;
}

void PreampDistorsion::prepareToPlay(juce::dsp::ProcessSpec &spec) {
    m_samplerate = spec.sampleRate;

    preGain.prepare(spec);
    stageGain.prepare(spec);
    postGain.prepare(spec);

    inputFilter.prepareToPlay(spec);
    inputFilter.setCoefficients(inputFilterFrequency);

    overSampler->initProcessing(spec.maximumBlockSize);

    preGain.setGainDecibels(0.0f);
    postGain.setGainDecibels(-12.0f);

    stageGain.setGainDecibels(20.0f);

    preGain.setRampDurationSeconds(0.02f);
    postGain.setRampDurationSeconds(0.02f);
}

void PreampDistorsion::process(AudioBlock &audioBlock) {

    float *bufferPtr = audioBlock.getChannelPointer(0);
    inputFilter.processBufferHighpass(bufferPtr, audioBlock.getNumSamples());
    

    overSampledBlock = overSampler->processSamplesUp(audioBlock);

    // d'abord sans upsampling pour voir si la courbe fonctionne bien.
        
    bufferPtr = overSampledBlock.getChannelPointer(0);
    for (size_t index = 0; index < overSampledBlock.getNumSamples(); index++) {
        
        bufferPtr[index] = preGain.processSample(bufferPtr[index]);

        bufferPtr[index] = stageGain.processSample(bufferPtr[index]);
        bufferPtr[index] = processDrive(bufferPtr[index], driveType);
        bufferPtr[index] = stageGain.processSample(bufferPtr[index]);
        bufferPtr[index] = processDrive(bufferPtr[index], driveType);
        bufferPtr[index] = stageGain.processSample(bufferPtr[index]);
        bufferPtr[index] = processDrive(bufferPtr[index], driveType);
        bufferPtr[index] = stageGain.processSample(bufferPtr[index]);
        bufferPtr[index] = processDrive(bufferPtr[index], driveType);

        bufferPtr[index] = postGain.processSample(bufferPtr[index]);
    }

    overSampler->processSamplesDown(audioBlock);
}

sample_t PreampDistorsion::processDrive(sample_t sample, DriveType curveType) {
    
    switch (curveType) {
        case APPROX:
            sample = tanhApprox(sample);
            break;

        case TANH:
            sample = tanh(sample);
            break;

        case CUBIC: 
            sample = sample <= -1 ? -0.666f 
                   : sample >= 1 ? 0.666f 
                   : sample - 0.333 * pow(sample, 3);
            break;

        case HARDCLIP:
            break;
    }

    return sample > 1.0f ? 1.0f
         : sample < -1.0f ? -1.0f
         : sample;
}
