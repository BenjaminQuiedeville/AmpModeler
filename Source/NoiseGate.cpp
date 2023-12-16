/*
==============================================================================

    NoiseGate.cpp
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

==============================================================================
*/

#include "NoiseGate.h"

void NoiseGate::prepareToPlay(double _samplerate) {

    samplerate = _samplerate;
    windowSize = floor(_samplerate*0.001f);

    rmsBuffer.setSize(1, windowSize);
    rmsBufferPtr = rmsBuffer.getWritePointer(0);
    rmsBufferIndex = 0;

    currentRms = 0.0f;
    threshold = -70.0f;

    isOpen = false;
    gateGain.init(0.0);
}


void NoiseGate::process(sample_t *input, sample_t *sidechain, size_t nSamples) {

    for (size_t index = 0; index < nSamples; index++) {

        rmsBufferPtr[rmsBufferIndex] = (sidechain[index]);
        rmsBufferIndex++;

        if (rmsBufferIndex == (size_t)rmsBuffer.getNumSamples()) {
            currentRms = rmsBuffer.getRMSLevel(0, 0, rmsBuffer.getNumSamples());
            rmsBufferIndex = 0;

            isOpen = juce::Decibels::gainToDecibels(currentRms) > threshold;
            
            // float rampTime = isOpen ? attackTimeSeconds : releaseTimeSeconds;
            // float newGain = isOpen ? 1.0f : 0.0f;
            gateGain.newTarget(isOpen ? 1.0 : 0.0, 
                               isOpen ? attackTimeMs : releaseTimeMs, 
                               samplerate);
        }

        input[index] *= gateGain.nextValue();
    }    
}
