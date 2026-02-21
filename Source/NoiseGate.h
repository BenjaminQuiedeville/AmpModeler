/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "common.h"


struct NoiseGate {
    
    ~NoiseGate() {
        free(rms.buffer.dataL);
        free(sideChain.dataL);
    }

    void prepareToPlay(float samplerate, u32 bufferSize) {
        local_const float gateBufferLengthMs = 10.0f;
        
        runningSum = 0.0;
        threshold = dbtoa(-75.0f);
        returnGain = threshold;
        holdCounter = 0;
        isOpen = false;

        reallocMonoBuffer(&rms.buffer, (u32)(samplerate * gateBufferLengthMs * 0.001f));
        
        rms.inv_length = 1.0f/(float)rms.buffer.size;
        rms.index = 0;

        reallocMonoBuffer(&gainBuffer, bufferSize);
        reallocMonoBuffer(&sideChain, bufferSize);
        
        attackCoeff = 1.0f;
        releaseCoeff = 1.0f;
        gateGain = 0.0f;
    }

    void process(Slice buffer, ChannelConfig config) {
        ZoneScoped;
           
        if (config == Stereo) {
            for (u32 index = 0; index < buffer.size; index++) {
                sideChain.dataL[index] = (buffer.dataL[index] + buffer.dataR[index]) * 0.5f;
            }
        } else {
            for (u32 index = 0; index < buffer.size; index++) {
                sideChain.dataL[index] = buffer.dataL[index];
            }
        }
        
        // optimisations
        // calculer toutes les valeurs de rms et de gain dans un rmsBuffer
        // multiplier avec bufferL/bufferR à la fin (plus rapide)
        
        for (u32 sampleIndex = 0; sampleIndex < buffer.size; sampleIndex++) {
            
            runningSum -= rms.buffer.dataL[rms.index];
            
            rms.buffer.dataL[rms.index] = sideChain.dataL[sampleIndex] * sideChain.dataL[sampleIndex];
            runningSum += rms.buffer.dataL[rms.index];
            if (runningSum < 0.0f) { runningSum = 0.0f; };            

            rms.index++;
            if (rms.index == rms.buffer.size) { rms.index = 0; }

            // if close && > thresh -> open 
            // if close && < thresh -> close
            // if open && > thresh - hyst -> open 
            // if open && < thresh -hyst -> close
            
            float rmsValue = sqrtf(runningSum * rms.inv_length);
            
            bool shouldOpen = false;
            if (rmsValue > threshold) { 
                shouldOpen = true; 
                holdCounter = 0;
            } else if (isOpen && rmsValue > returnGain) { 
                shouldOpen = true;
                holdCounter = 0; 
            } else if (isOpen && rmsValue < returnGain) {
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
                        
            gainBuffer.dataL[sampleIndex] = gateGain;
        }
        
        for (u32 index = 0; index < buffer.size; index++) {
            buffer.dataL[index] *= gainBuffer.dataL[index];
        }        
    
        if (buffer.dataR) {
            for (u32 index = 0; index < buffer.size; index++) {
                buffer.dataR[index] *= gainBuffer.dataL[index];
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

    Slice gainBuffer = {};
    Slice sideChain = {};

    float runningSum = 0.0f;
    struct {
        Slice buffer = {};
        u32 index = 0;
        float inv_length = 0.0f;
    } rms;
};
