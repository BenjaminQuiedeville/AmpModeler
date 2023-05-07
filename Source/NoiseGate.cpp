/*
  ==============================================================================

    NoiseGate.cpp
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

  ==============================================================================
*/

#include "NoiseGate.h"

void NoiseGate::prepareToPlay(juce::dsp::ProcessSpec &spec) {

    samplerate = spec.sampleRate;
    windowSize = floor(samplerate*0.001f);

    rmsBuffer.setSize(1, windowSize);
    rmsBufferPtr = rmsBuffer.getWritePointer(0);
    rmsBufferIndex = 0;

    currentRms = 0.0f;
    threshold = -70.0f;

    isOpen = false;
    gateGainLeft.prepare(spec);
    gateGainLeft.setRampDurationSeconds(attackSeconds);
    gateGainLeft.setGainLinear(0.0f);

    gateGainRight.prepare(spec);
    gateGainRight.setRampDurationSeconds(attackSeconds);
    gateGainRight.setGainLinear(0.0f);
}


void NoiseGate::process(AudioBlock &audioBlock) {

    float *audioBlockPtrLeft = audioBlock.getChannelPointer(0);
    float *audioBlockPtrRight = audioBlock.getChannelPointer(1);

    for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {

        rmsBufferPtr[rmsBufferIndex] = (audioBlockPtrLeft[index] + audioBlockPtrRight[index]);
        rmsBufferIndex++;

        if (rmsBufferIndex == rmsBuffer.getNumSamples()) {
            currentRms = rmsBuffer.getRMSLevel(0, 0, rmsBuffer.getNumSamples());
            rmsBufferIndex = 0;

            isOpen = juce::Decibels::gainToDecibels(currentRms) > threshold;
            // DBG("current RMS : " << juce::Decibels::gainToDecibels(currentRms));
            
            float rampTime = isOpen ? attackSeconds : releaseSeconds;
            float newGain = isOpen ? 1.0f : 0.0f;
            gateGainLeft.setRampDurationSeconds(rampTime);
            gateGainLeft.setGainLinear(newGain);

            gateGainRight.setRampDurationSeconds(rampTime);
            gateGainRight.setGainLinear(newGain);
        }

       audioBlockPtrLeft[index] = gateGainLeft.processSample(audioBlockPtrLeft[index]);
       audioBlockPtrRight[index] = gateGainRight.processSample(audioBlockPtrRight[index]);
    }    
}

float NoiseGate::computeRampIncrement(float currentValue, float targetValue, float nSamples) {
    return (targetValue - currentValue)/(nSamples);
}

float NoiseGate::rampStep(float currentValue, float targetValue, float increment) {
    if (fabs(targetValue - currentValue) < fabs(increment)) {
        return targetValue;
    }
    return currentValue + increment;
}