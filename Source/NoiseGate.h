/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "common.h"
#include "SmoothParam.h"

#define GATE_BUFFER_LENGTH_SECONDS 0.1

struct NoiseGate {
    
    ~NoiseGate() {
    
        free(gateBuffer);
    }


    void prepareToPlay(double _samplerate) {

        samplerate = _samplerate;
        gateBufferLength = (size_t)(samplerate * GATE_BUFFER_LENGTH_SECONDS);
        gateBufferIndex = 0;
        absoluteSum = 0.0;
        threshold = DB_TO_GAIN(-70.0);

        if (gateBuffer == nullptr) {
            gateBuffer = (sample_t *)calloc(gateBufferLength, sizeof(sample_t));

        } else {
            memset(gateBuffer, 0, gateBufferLength * sizeof(sample_t));
        }
        
        gateGain.init(0.0);
    }

    void process(sample_t *input, sample_t *sidechain, size_t nSamples) {

        for (size_t i = 0; i < nSamples; i++) {
            
            absoluteSum -= std::abs(gateBuffer[gateBufferIndex]);
            
            gateBuffer[gateBufferIndex] = sidechain[i];
            absoluteSum += std::abs(gateBuffer[gateBufferIndex]);
            
            gateBufferIndex = (gateBufferIndex+1) % gateBufferLength;

            bool isOpen = (absoluteSum / gateBufferLength) > threshold;
            gateGain.newTarget(isOpen ? 1.0 : 0.0, 
                                isOpen ? attackTimeMs : releaseTimeMs,
                                samplerate);
            
            input[i] *= (sample_t)gateGain.nextValue();
                        
        }    
    }

    double samplerate = 0.0;
    double threshold = 0.0;
    
    sample_t *gateBuffer = nullptr;
    size_t gateBufferLength = 0;
    s32 gateBufferIndex = 0;
    
    double absoluteSum = 0.0;
    
    SmoothParamIIR gateGain;
    
    double attackTimeMs = 1.0;
    double releaseTimeMs = 15.0;

};



