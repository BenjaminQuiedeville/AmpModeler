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

        bool reallocBuffer = _samplerate != samplerate;

        samplerate = _samplerate;
        gateBufferLength = (u32)(samplerate * GATE_BUFFER_LENGTH_SECONDS);
        gateBufferIndex = 0;
        absoluteSum = 0.0;
        threshold = dbtoa(-75.0);
        returnGain = threshold;
        isOpen = false;

        if (gateBuffer == nullptr) {
            gateBuffer = (float *)calloc(gateBufferLength, sizeof(float));

        } else {
            
            if (reallocBuffer) {
                free(gateBuffer);
                gateBuffer = (float*)malloc(gateBufferLength * sizeof(float));
            }
            memset(gateBuffer, 0, gateBufferLength * sizeof(float));            
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
            double amplitude = absoluteSum / gateBufferLength;
            
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
                gateGain.newTarget(1.0, attackTimeMs, samplerate);
                isOpen = true;
                
            } else if (!shouldOpen && isOpen) {
                gateGain.newTarget(0.0, releaseTimeMs, samplerate);
                isOpen = false;
            }
            
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
    u32 gateBufferLength = 0;
    u32 gateBufferIndex = 0;
    
    double absoluteSum = 0.0;
    
    SmoothParamIIR gateGain;
    
    double attackTimeMs = 1.0;
    double releaseTimeMs = 15.0;
    double hysteresis = 0.0;
    double returnGain = 0.0;
    bool   isOpen = false;
};
