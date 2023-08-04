/*
  ==============================================================================

    PreampDistorsion.cpp
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreampDistorsion.h"

PreampDistorsion::PreampDistorsion() {
    
    overSampler = new juce::dsp::Oversampling<sample_t>(
        1, 1, 
        juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR
    );

    driveType = CUBIC;
}

PreampDistorsion::~PreampDistorsion() {
    delete overSampler;
}

void PreampDistorsion::prepareToPlay(juce::dsp::ProcessSpec &spec) {
    samplerate = spec.sampleRate;

    preGain.init(samplerate, 0.02, 0.0, SmoothParam::CurveType::LIN);
    postGain.init(samplerate, 0.02, 0.0, SmoothParam::CurveType::LIN);

    inputFilter.prepareToPlay(spec.sampleRate);
    inputFilter.setCoefficients(inputFilterFrequency);

    overSampler->initProcessing(spec.maximumBlockSize);

    preGain.newTarget(DB_TO_GAIN(0.0));
    postGain.newTarget(DB_TO_GAIN(-12.0));

    stageGain = DB_TO_GAIN(20.0);
}

void PreampDistorsion::process(AudioBlock &audioBlock) {

    float *bufferPtr = audioBlock.getChannelPointer(0);
    inputFilter.processBufferHighpass(bufferPtr, audioBlock.getNumSamples());
    
    // const AudioBlock& audioBlockToUpSample = audioBlock;

    // overSampledBlock = overSampler->processSamplesUp(audioBlockToUpSample);

    // d'abord sans upsampling pour voir si la courbe fonctionne bien.
        
    bufferPtr = audioBlock.getChannelPointer(0);
    for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {
        
        bufferPtr[index] = preGain.nextValue() * bufferPtr[index];

        bufferPtr[index] = stageGain * bufferPtr[index];
        bufferPtr[index] = processDrive(bufferPtr[index], driveType);

        bufferPtr[index] = stageGain * bufferPtr[index];
        bufferPtr[index] = processDrive(bufferPtr[index], driveType);

        bufferPtr[index] = postGain.nextValue() * bufferPtr[index];
    }

    // overSampler->processSamplesDown(audioBlock);
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
