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
    gateGain.prepare(spec);
    gateGain.setRampDurationSeconds(attackSeconds);
    gateGain.setGainLinear(0.0f);
}


void NoiseGate::process(AudioBlock &audioBlock) {

    float *audioBlockPtr = audioBlock.getChannelPointer(0);

    for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {

        rmsBufferPtr[rmsBufferIndex] = (audioBlockPtr[index]);
        rmsBufferIndex++;

        if (rmsBufferIndex == rmsBuffer.getNumSamples()) {
            currentRms = rmsBuffer.getRMSLevel(0, 0, rmsBuffer.getNumSamples());
            rmsBufferIndex = 0;

            isOpen = juce::Decibels::gainToDecibels(currentRms) > threshold;
            // DBG("current RMS : " << juce::Decibels::gainToDecibels(currentRms));
            
            float rampTime = isOpen ? attackSeconds : releaseSeconds;
            float newGain = isOpen ? 1.0f : 0.0f;
            gateGain.setRampDurationSeconds(rampTime);
            gateGain.setGainLinear(newGain);
        }

       audioBlockPtr[index] = gateGain.processSample(audioBlockPtr[index]);
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