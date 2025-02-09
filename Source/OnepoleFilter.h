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
        ZoneScoped;
        b0 = 1.0f;
        a1 = 0.0f;
        y1L = 0.0f;
        y1R = 0.0f;
    }

    void setCoefficients(double frequency, double samplerate) {
        ZoneScoped;
        b0 = sinf(M_PI / samplerate * frequency);
        a1 = b0 - 1.0f;
    }

    void processSingleSampleLowPass(float *sample) {
        ZoneScoped;
        *sample = *sample * b0 - a1 * y1L;
        y1L = *sample;
    }

    void processLowpass(float *bufferL, float *bufferR, u64 numSamples) {
        ZoneScoped;
        if (bufferL) {
            for (u64 index = 0; index < numSamples; index++) {
                bufferL[index] = bufferL[index] * b0 - a1 * y1L;
                y1L = bufferL[index];
            }
        }
        
        if (bufferR) {
            for (u64 index = 0; index < numSamples; index++) {
                bufferR[index] = bufferR[index] * b0 - a1 * y1R;
                y1R = bufferR[index];
            }
        }
    }

    void processHighpass(float *bufferL, float *bufferR, u64 numSamples) {
        ZoneScoped;
        if (bufferL) {
            for (u64 index = 0; index < numSamples; index++) {
                float lpSample = bufferL[index] * b0 - a1 * y1L;
                y1L = lpSample;
                bufferL[index] -= lpSample;
            }
        }
        
        if (bufferR){
            for (u64 index = 0; index < numSamples; index++) {
                float lpSample = bufferR[index] * b0 - a1 * y1R;
                y1R = lpSample;
                bufferR[index] -= lpSample;
            }
        }
    }
        
    float b0 = 1.0;
    float a1 = 0.0;
    float y1L = 0.0f;
    float y1R = 0.0f;
};

#endif // ONEPOLE_H
