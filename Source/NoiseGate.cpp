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
    gateBufferLength = (size_t)(samplerate * GATE_BUFFER_LENGTH_SECONDS);
    gateBufferIndex = 0;
    absoluteSum = 0.0;
    threshold = -70.0;

    if (gateBuffer == nullptr) {
        gateBuffer = (sample_t *)malloc(gateBufferLength *  sizeof(sample_t));
    } else {
        for (size_t i = 0; i < gateBufferLength; i++) {
            gateBuffer[i] = 0.0f;
        }
    }
    
    gateGain->init(0.0);
}


void NoiseGate::process(sample_t *input, sample_t *sidechain, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {
        
        absoluteSum -= std::abs(gateBuffer[gateBufferIndex];
        
        gateBuffer[gateBufferIndex] = sidechain[i];
        gateBufferIndex = (gateBufferIndex+1) % gateBufferLength;
        
        absoluteSum += std::abs(gateBuffer[gateBufferIndex];
                
        bool isOpen = (absoluteSum / gateBufferLength) > threshold;
        gateGain->newTarget(isOpen ? 1.0 : 0.0, 
                            isOpen ? attackTimeMs : releaseTimeMs,
                            samplerate);
        
        input[i] *= (sample_t)gateGain->nextValue();
                       
    }    
}
