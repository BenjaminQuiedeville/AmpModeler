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
            gateBuffer = (float *)calloc(gateBufferLength, sizeof(float));

        } else {
            memset(gateBuffer, 0, gateBufferLength * sizeof(float));
        }
        
        gateGain.init(0.0);
    }

    void process(float *bufferL, float *bufferR, float *sidechain, size_t nSamples) {
        ZoneScoped;
        
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
            
            if (amplitude > threshold) { 
                isOpen = true;  
            }
            
            if (isOpen && amplitude > returnGain) { 
                isOpen = true;  
            }
            
            if (amplitude < returnGain) { 
                isOpen = false; 
            }
            
            
            gateGain.newTarget(isOpen ? 1.0 : 0.0, 
                               isOpen ? attackTimeMs : releaseTimeMs,
                               samplerate);
            
            float gateGainValue = (float)gateGain.nextValue(); 
            
            bufferL[i] *= gateGainValue;
            
            if (bufferR) {
                bufferR[i] *= gateGainValue;
            }
        }
    }

    double samplerate = 0.0;
    double threshold = 0.0;
    
    float *gateBuffer = nullptr;
    size_t gateBufferLength = 0;
    s32 gateBufferIndex = 0;
    
    double absoluteSum = 0.0;
    
    SmoothParamIIR gateGain;
    
    double attackTimeMs = 1.0;
    double releaseTimeMs = 15.0;
    double hysteresis = 0.0;
    double returnGain = 0.0;
};
