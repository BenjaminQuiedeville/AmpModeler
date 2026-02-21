/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "common.h"


struct NoiseGate {
    
    ~NoiseGate() {
        free(buffer.data);
    }

    void prepareToPlay(float samplerate) {
        local_const float gateBufferLengthMs = 10.0f;
        
        buffer.length = (u32)(samplerate * gateBufferLengthMs * 0.001f);
        buffer.inv_length = 1.0f/(float)buffer.length;
        buffer.index = 0;
        runningSum = 0.0;
        threshold = dbtoa(-75.0f);
        returnGain = threshold;
        holdCounter = 0;
        isOpen = false;

        if (buffer.data) { free(buffer.data); }
        buffer.data = allocFloat(buffer.length);
        memsetZeroFloat(buffer.data, buffer.length);            
        
        attackCoeff = 1.0f;
        releaseCoeff = 1.0f;
        gateGain = 0.0f;
    }

    void process(float *bufferL, float *bufferR, float *sideChain, u32 nSamples) {
        ZoneScoped;
        
        for (u32 i = 0; i < nSamples; i++) {
            
            runningSum -= buffer.data[buffer.index];
            
            buffer.data[buffer.index] = sideChain[i] * sideChain[i];
            runningSum += buffer.data[buffer.index];
            if (runningSum < 0.0f) { runningSum = 0.0f; };            

            buffer.index++;
            if (buffer.index == buffer.length) { buffer.index = 0; }

            // if close && > thresh -> open 
            // if close && < thresh -> close
            // if open && > thresh - hyst -> open 
            // if open && < thresh -hyst -> close
            
            float rms = sqrtf(runningSum * buffer.inv_length);
            
            bool shouldOpen = false;
            if (rms > threshold) { 
                shouldOpen = true; 
                holdCounter = 0;
            } else if (isOpen && rms > returnGain) { 
                shouldOpen = true;
                holdCounter = 0; 
            } else if (isOpen && rms < returnGain) {
                holdCounter++;
                shouldOpen = true;
                if (holdCounter >= holdCounterMax) { 
                    holdCounter = 0;
                    shouldOpen = false; 
                }
            } else {
                holdCounter = 0;
            }
                                    
            float gainCoeff = 0.0f;
            float target = 0.0f;
            if (shouldOpen) {
                gainCoeff = attackCoeff;
                target = 1.0f;
                isOpen = true;                                
            } else {
                gainCoeff = releaseCoeff;
                target = 0.0f;
                isOpen = false;
            }
                        
            gateGain = (1.0f - gainCoeff) * gateGain + gainCoeff * target;
                        
            bufferL[i] *= gateGain;
            
            if (bufferR) {
                bufferR[i] *= gateGain;
            }
        }
    }

    float threshold = 0.0;
    float returnGain = 0.0;
    u32 holdCounter = 0;
    u32 holdCounterMax = 48 * 5;
    
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float gateGain = 0.0;
    
    bool isOpen = false;

    float runningSum = 0.0f;
    struct {
        float *data = nullptr;
        u32 index = 0;
        u32 length = 0;
        float inv_length = 0.0f;
    } buffer;
};
