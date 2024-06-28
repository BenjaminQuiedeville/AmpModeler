/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef ONEPOLE_H
#define ONEPOLE_H

#include "juce_core/juce_core.h"
#include "common.h"


struct OnepoleFilter {

    void prepareToPlay() {
        b0 = 1.0;
        a1 = 0.0;
        y1L = 0.0f;
        y1R = 0.0f;
    }

    void setCoefficients(double frequency, double samplerate) {
        b0 = sin(juce::MathConstants<double>::pi / samplerate * frequency);
        a1 = b0 - 1.0;
    }

    void processLowpass(Sample *bufferL, Sample *bufferR, u64 numSamples) {
        
        if (bufferL) {
            for (u64 index = 0; index < numSamples; index++) {
                bufferL[index] = (Sample)(bufferL[index] * b0 - a1 * y1L);
                y1L = bufferL[index];                    
            }
        }
        
        if (bufferR) {
            for (u64 index = 0; index < numSamples; index++) {
                bufferR[index] = (Sample)(bufferR[index] * b0 - a1 * y1R);
                y1R = bufferR[index];                    
            }
        }                
    }

    void processHighpass(Sample *bufferL, Sample *bufferR, u64 numSamples) {
        
        if (bufferL) {
            for (u64 index = 0; index < numSamples; index++) {
                Sample lpSample = (Sample)(bufferL[index] * b0 - a1 * y1L);
                y1L = lpSample;
                bufferL[index] -= lpSample;
            }
        }    
        
        if (bufferR){
            for (u64 index = 0; index < numSamples; index++) {
                Sample lpSample = (Sample)(bufferR[index] * b0 - a1 * y1R);
                y1R = lpSample;
                bufferR[index] -= lpSample;
            }
        }    
    }
    
    
    double b0 = 1.0;
    double a1 = 0.0;
    Sample y1L = 0.0f;
    Sample y1R = 0.0f;
};

#endif // ONEPOLE_H
