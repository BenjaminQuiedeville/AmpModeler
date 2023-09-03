/*
==============================================================================

    NoiseGate.cpp
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

==============================================================================
*/

#include "NoiseGate.h"

void NoiseGate::prepareToPlay(double samplerate) {

    samplerate = samplerate;
    windowSize = floor(samplerate*0.001f);

    rmsBuffer.setSize(1, windowSize);
    rmsBufferPtr = rmsBuffer.getWritePointer(0);
    rmsBufferIndex = 0;

    currentRms = 0.0f;
    threshold = -70.0f;

    isOpen = false;
    gateGain.init(samplerate, attackTimeMs, 0.0, SMOOTH_PARAM_LIN);
}


void NoiseGate::process(float *input, size_t nSamples) {

    for (size_t index = 0; index < nSamples; index++) {

        rmsBufferPtr[rmsBufferIndex] = (input[index]);
        rmsBufferIndex++;

        if (rmsBufferIndex == (size_t)rmsBuffer.getNumSamples()) {
            currentRms = rmsBuffer.getRMSLevel(0, 0, rmsBuffer.getNumSamples());
            rmsBufferIndex = 0;

            isOpen = juce::Decibels::gainToDecibels(currentRms) > threshold;
            // DBG("current RMS : " << juce::Decibels::gainToDecibels(currentRms));
            
            // float rampTime = isOpen ? attackTimeSeconds : releaseTimeSeconds;
            // float newGain = isOpen ? 1.0f : 0.0f;
            gateGain.rampTimeMs = isOpen ? attackTimeMs : releaseTimeMs;
            gateGain.newTarget(isOpen ? 1.0 : 0.0);
        }

        input[index] = gateGain.nextValue() * input[index];
    }    
}
