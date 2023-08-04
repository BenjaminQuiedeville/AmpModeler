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
    gateGain.init(spec.sampleRate, attackTimeMs, 0.0, SmoothParam::CurveType::LIN);
}


void NoiseGate::process(AudioBlock &audioBlock) {

    float *audioBlockPtr = audioBlock.getChannelPointer(0);
    for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {

        rmsBufferPtr[rmsBufferIndex] = (audioBlockPtr[index]);
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

        audioBlockPtr[index] = gateGain.nextValue() * audioBlockPtr[index];
    }    
}
