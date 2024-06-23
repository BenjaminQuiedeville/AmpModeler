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
        returnGain = threshold;

        if (gateBuffer == nullptr) {
            gateBuffer = (Sample *)calloc(gateBufferLength, sizeof(Sample));

        } else {
            memset(gateBuffer, 0, gateBufferLength * sizeof(Sample));
        }
        
        gateGain.init(0.0);
    }

    void process(Sample *bufferL, Sample *bufferR, Sample *sidechain, size_t nSamples) {

        for (size_t i = 0; i < nSamples; i++) {
            
            absoluteSum -= std::abs(gateBuffer[gateBufferIndex]);
            
            gateBuffer[gateBufferIndex] = sidechain[i];
            absoluteSum += std::abs(gateBuffer[gateBufferIndex]);
            
            gateBufferIndex++;
            if (gateBufferIndex == gateBufferLength) { gateBufferIndex = 0; }

            // if close && > thresh -> open 
            // if close && < thresh -> close
            // if open && > thresh - hyst -> open 
            // if open && < thresh -hyst -> close
            
            bool isOpen = false;
            double amplitude = absoluteSum / gateBufferLength;
            
            if (amplitude < returnGain)           { isOpen = false; }
            if (amplitude > threshold)            { isOpen = true; }
            if (isOpen && amplitude > returnGain) { isOpen = true; }
            
            gateGain.newTarget(isOpen ? 1.0 : 0.0, 
                               isOpen ? attackTimeMs : releaseTimeMs,
                               samplerate);
            
            Sample gateGainValue = (Sample)gateGain.nextValue(); 
            
            bufferL[i] *= gateGainValue;
            
            if (bufferR) {
                bufferR[i] *= gateGainValue;
            }
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
    double hysteresis = 0.0;
    double returnGain = 0.0;
};
