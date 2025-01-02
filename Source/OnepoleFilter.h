/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef ONEPOLE_H
#define ONEPOLE_H

#include "common.h"


struct OnepoleFilter {

    void prepareToPlay() {
        b0 = 1.0;
        a1 = 0.0;
        y1L = 0.0f;
        y1R = 0.0f;
    }

    void setCoefficients(double frequency, double samplerate) {
        b0 = sin(M_PI / samplerate * frequency);
        a1 = b0 - 1.0;
    }

    void processLowpass(float *bufferL, float *bufferR, u64 numSamples) {
        
        if (bufferL) {
            for (u64 index = 0; index < numSamples; index++) {
                bufferL[index] = (float)(bufferL[index] * b0 - a1 * y1L);
                y1L = bufferL[index];                    
            }
        }
        
        if (bufferR) {
            for (u64 index = 0; index < numSamples; index++) {
                bufferR[index] = (float)(bufferR[index] * b0 - a1 * y1R);
                y1R = bufferR[index];                    
            }
        }                
    }

    void processHighpass(float *bufferL, float *bufferR, u64 numSamples) {
        
        if (bufferL) {
            for (u64 index = 0; index < numSamples; index++) {
                float lpSample = (float)(bufferL[index] * b0 - a1 * y1L);
                y1L = lpSample;
                bufferL[index] -= lpSample;
            }
        }    
        
        if (bufferR){
            for (u64 index = 0; index < numSamples; index++) {
                float lpSample = (float)(bufferR[index] * b0 - a1 * y1R);
                y1R = lpSample;
                bufferR[index] -= lpSample;
            }
        }    
    }
    
    
    double b0 = 1.0;
    double a1 = 0.0;
    float y1L = 0.0f;
    float y1R = 0.0f;
};

#endif // ONEPOLE_H
