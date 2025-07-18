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


    void prepareToPlay(float _samplerate) {

        bool reallocBuffer = _samplerate != samplerate;

        samplerate = _samplerate;
        gateBufferLength = (u32)(samplerate * GATE_BUFFER_LENGTH_SECONDS);
        gateBufferIndex = 0;
        absoluteSum = 0.0;
        threshold = (float)dbtoa(-75.0);
        returnGain = threshold;
        isOpen = false;

        if (gateBuffer == nullptr) {
            gateBuffer = (float *)calloc(gateBufferLength, sizeof(float));

        } else {
            
            if (reallocBuffer) {
                free(gateBuffer);
                gateBuffer = (float*)calloc(gateBufferLength, sizeof(float));
            }
            FLOAT_CLEAR(gateBuffer, gateBufferLength);            
        }
        
        gateGain.init(0.0);
    }

    void process(float *bufferL, float *bufferR, float *sidechain, u32 nSamples) {
        ZoneScoped;
        
        for (u32 i = 0; i < nSamples; i++) {
            
            absoluteSum -= std::abs(gateBuffer[gateBufferIndex]);
            
            gateBuffer[gateBufferIndex] = sidechain[i];
            absoluteSum += std::abs(gateBuffer[gateBufferIndex]);
            
            gateBufferIndex++;
            if (gateBufferIndex == gateBufferLength) { gateBufferIndex = 0; }

            // if close && > thresh -> open 
            // if close && < thresh -> close
            // if open && > thresh - hyst -> open 
            // if open && < thresh -hyst -> close
            
            bool shouldOpen = false;
            float amplitude = absoluteSum / (float)gateBufferLength;
            
            if (amplitude > threshold) { 
                shouldOpen = true;  
            }
            
            if (isOpen && amplitude > returnGain) { 
                shouldOpen = true;  
            }
            
            if (amplitude < returnGain) { 
                shouldOpen = false; 
            }
            
            if (shouldOpen && !isOpen) {
                gateGain.newTarget(1.0f, attackTimeMs, samplerate);
                isOpen = true;
                
            } else if (!shouldOpen && isOpen) {
                gateGain.newTarget(0.0f, releaseTimeMs, samplerate);
                isOpen = false;
            }
            
            float gateGainValue = gateGain.nextValue(); 
            
            bufferL[i] *= gateGainValue;
            
            if (bufferR) {
                bufferR[i] *= gateGainValue;
            }
        }
    }

    float samplerate = 0.0;
    float threshold = 0.0;
    
    float *gateBuffer = nullptr;
    u32 gateBufferLength = 0;
    u32 gateBufferIndex = 0;
    
    float absoluteSum = 0.0;
    
    SmoothParamIIR gateGain;
    
    float attackTimeMs = 1.0;
    float releaseTimeMs = 15.0;
    float hysteresis = 0.0;
    float returnGain = 0.0;
    bool   isOpen = false;
};
