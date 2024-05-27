/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "common.h"
#include "SmoothParam.h"

#define GATE_BUFFER_LENGTH_SECONDS 0.01

struct NoiseGate {
    
    ~NoiseGate() {
    
        free(gateBuffer);
    }


    void prepareToPlay(double _samplerate) {

        samplerate = _samplerate;
        gateBufferLength = (size_t)(samplerate * GATE_BUFFER_LENGTH_SECONDS);
        gateBufferIndex = 0;
        absoluteSum = 0.0;
        threshold = dbtoa(-70.0);

        if (gateBuffer == nullptr) {
            gateBuffer = (Sample *)calloc(gateBufferLength, sizeof(Sample));

        } else {
            memset(gateBuffer, 0, gateBufferLength * sizeof(Sample));
        }
        
        gateGain.init(0.0);
    }

    void process(Sample *input, Sample *sidechain, size_t nSamples) {

        for (size_t i = 0; i < nSamples; i++) {
            
            absoluteSum -= std::abs(gateBuffer[gateBufferIndex]);
            
            gateBuffer[gateBufferIndex] = sidechain[i];
            absoluteSum += std::abs(gateBuffer[gateBufferIndex]);
            
            gateBufferIndex++;
            if (gateBufferIndex == gateBufferLength) { gateBufferIndex = 0; }

            bool isOpen = (absoluteSum / gateBufferLength) > threshold;
            gateGain.newTarget(isOpen ? 1.0 : 0.0, 
                               isOpen ? attackTimeMs : releaseTimeMs,
                               samplerate);
            
            input[i] *= (Sample)gateGain.nextValue();
        }    
    }

    double samplerate = 0.0;
    double threshold = 0.0;
    
    Sample *gateBuffer = nullptr;
    size_t gateBufferLength = 0;
    s32 gateBufferIndex = 0;
    
    double absoluteSum = 0.0;
    
    SmoothParamIIR gateGain;
    
    double attackTimeMs = 1.0;
    double releaseTimeMs = 15.0;

};
