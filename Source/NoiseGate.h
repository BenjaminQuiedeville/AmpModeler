/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "common.h"


struct NoiseGate {
    
    ~NoiseGate() {
        free(rmsBuffer.data);
    }

    void prepareToPlay(float samplerate, u32 bufferSize) {
        local_const float gateBufferLengthMs = 10.0f;
        
        rmsBuffer.length = (u32)(samplerate * gateBufferLengthMs * 0.001f);
        rmsBuffer.inv_length = 1.0f/(float)rmsBuffer.length;
        rmsBuffer.index = 0;
        runningSum = 0.0;
        threshold = dbtoa(-75.0f);
        returnGain = threshold;
        holdCounter = 0;
        isOpen = false;

        if (rmsBuffer.data) { free(rmsBuffer.data); }
        rmsBuffer.data = allocFloat(rmsBuffer.length);
        memsetZeroFloat(rmsBuffer.data, rmsBuffer.length);
        
        if (gainBuffer.data) { free(gainBuffer.data); }
        gainBuffer.size = bufferSize;
        gainBuffer.data = allocFloat(gainBuffer.size);
        memsetZeroFloat(gainBuffer.data, gainBuffer.size);
        
        attackCoeff = 1.0f;
        releaseCoeff = 1.0f;
        gateGain = 0.0f;
    }

    void process(float *bufferL, float *bufferR, float *sideChain, u32 nSamples) {
        ZoneScoped;
        
        // optimisations
        // calculer toutes les valeurs de rms et de gain dans un rmsBuffer
        // multiplier avec bufferL/bufferR à la fin (plus rapide)
        
        for (u32 sampleIndex = 0; sampleIndex < nSamples; sampleIndex++) {
            
            runningSum -= rmsBuffer.data[rmsBuffer.index];
            
            rmsBuffer.data[rmsBuffer.index] = sideChain[sampleIndex] * sideChain[sampleIndex];
            runningSum += rmsBuffer.data[rmsBuffer.index];
            if (runningSum < 0.0f) { runningSum = 0.0f; };            

            rmsBuffer.index++;
            if (rmsBuffer.index == rmsBuffer.length) { rmsBuffer.index = 0; }

            // if close && > thresh -> open 
            // if close && < thresh -> close
            // if open && > thresh - hyst -> open 
            // if open && < thresh -hyst -> close
            
            float rms = sqrtf(runningSum * rmsBuffer.inv_length);
            
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
                        
            gainBuffer.data[sampleIndex] = gateGain;
        }
        
        for (u32 index = 0; index < nSamples; index++) {
            bufferL[index] *= gainBuffer.data[index];
        }        
    
        if (bufferR) {
            for (u32 index = 0; index < nSamples; index++) {
                bufferR[index] *= gainBuffer.data[index];
            }        
        }
    }

    float threshold = 0.0f;
    float returnGain = 0.0f;
    u32 holdCounter = 0;
    u32 holdCounterMax = 0;
    
    float attackCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    float gateGain = 0.0;
    
    bool isOpen = false;

    struct {
        float *data = nullptr;
        u32 size = 0;
    } gainBuffer;

    float runningSum = 0.0f;
    struct {
        float *data = nullptr;
        u32 index = 0;
        u32 length = 0;
        float inv_length = 0.0f;
    } rmsBuffer;
};
