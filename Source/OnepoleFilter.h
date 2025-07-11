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
        ZoneScoped;
        b0 = (float)sin(M_PI / samplerate * frequency);
        a1 = b0 - 1.0f;
    }

    void processLowpass(float *bufferL, float *bufferR, u64 numSamples) {
        ZoneScoped;
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
        ZoneScoped;
        
        float lb0 = b0;
        float la1 = a1;

        if (bufferL) {
            float ly1L = y1L;
            
            for (u64 index = 0; index < numSamples; index++) {
                float lpSample = (float)(bufferL[index] * lb0 - la1 * ly1L);
                ly1L = lpSample;
                bufferL[index] -= lpSample;
            }
            
            y1L = ly1L;
        }    
        
        if (bufferR){
            float ly1R = y1R;
            
            for (u64 index = 0; index < numSamples; index++) {
                float lpSample = (float)(bufferR[index] * lb0 - la1 * ly1R);
                ly1R = lpSample;
                bufferR[index] -= lpSample;
            }
            y1R = ly1R;
        }    
    }
    
    
    float b0 = 1.0;
    float a1 = 0.0;
    float y1L = 0.0f;
    float y1R = 0.0f;
};

#endif // ONEPOLE_H
