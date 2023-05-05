/*
  ==============================================================================

    PreampDistorsion.cpp
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreampDistorsion.h"

PreampDistorsion::PreampDistorsion() : 
    overSampler(2, 2, juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR)
{

    preGain.setGainDecibels(0.0f);
    postGain.setGainDecibels(-24.0f);

    preGain.setRampDurationSeconds(0.02f);
    postGain.setRampDurationSeconds(0.02f);


    driveType = APPROX;
}

void PreampDistorsion::prepareToPlay(juce::dsp::ProcessSpec &spec)
{
    m_samplerate = spec.sampleRate;

    inputFilter.prepareToPlay(spec);
    inputFilter.setCoefficients(inputFilterFrequency);
    
    overSampler.initProcessing(spec.maximumBlockSize);

}

void PreampDistorsion::process(AudioBlock &audioBlock)
{
    // d'abord sans upsampling pour voir si la courbe fonctionne bien.

    inputFilter.processBuffer(audioBlock, OnepoleFilter::Type::LOWPASS);

    for (uint8_t channel = 0; channel < audioBlock.getNumChannels(); channel++) {
        
        float *bufferPtr = audioBlock.getChannelPointer(channel);
        for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {
            
            bufferPtr[index] = preGain.processSample(bufferPtr[index]);

            bufferPtr[index] = processDrive(bufferPtr[index], driveType); 
            bufferPtr[index] = processDrive(bufferPtr[index], driveType); 
            bufferPtr[index] = processDrive(bufferPtr[index], driveType); 
            bufferPtr[index] = processDrive(bufferPtr[index], driveType); 

            bufferPtr[index] = postGain.processSample(bufferPtr[index]);
        }
    }
}

sample_t PreampDistorsion::processDrive(sample_t &sample, DriveType curveType) {
    
    switch (curveType) {
        case APPROX:
            sample = tanhApprox(sample);
            break;

        case TANH:
            sample = tanh(sample);
            break;

        case HARDCLIP:
            break;
    }

    return sample > 1.0f ? 1.0f
         : sample < -1.0f ? -1.0f
         : sample;
}
