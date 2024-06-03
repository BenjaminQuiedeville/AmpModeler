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

    void processLowpassLeft(Sample *buffer, u64 numSamples) {
        for (u64 index = 0; index < numSamples; index++) {
            buffer[index] = (Sample)(buffer[index] * b0 - a1 * y1L);
            y1L = buffer[index];                    
        }
    }

    void processLowpassRight(Sample *buffer, u64 numSamples) {
        for (u64 index = 0; index < numSamples; index++) {
            buffer[index] = (Sample)(buffer[index] * b0 - a1 * y1R);
            y1R = buffer[index];                    
        }
    }

    void processLowpassStereo(Sample *bufferL, Sample *bufferR, u64 numSamples) {
        for (u64 index = 0; index < numSamples; index++) {
            bufferL[index] = (Sample)(bufferL[index] * b0 - a1 * y1L);
            y1L = bufferL[index];                    
            
            bufferR[index] = (Sample)(bufferR[index] * b0 - a1 * y1R);
            y1R = bufferR[index];                    
        }
    }


    void processHighpassLeft(Sample *buffer, u64 numSamples) {
        for (u64 index = 0; index < numSamples; index++) {
            Sample lpSample = (Sample)(buffer[index] * b0 - a1 * y1L);
            y1L = lpSample;
            buffer[index] -= lpSample;
        }
    }

    void processHighpassRight(Sample *buffer, u64 numSamples) {
        for (u64 index = 0; index < numSamples; index++) {
            Sample lpSample = (Sample)(buffer[index] * b0 - a1 * y1R);
            y1R = lpSample;
            buffer[index] -= lpSample;
        }
    }

    void processHighpassStereo(Sample *bufferL, Sample *bufferR, u64 numSamples) {
        for (u64 index = 0; index < numSamples; index++) {
            Sample lpSample = (Sample)(bufferL[index] * b0 - a1 * y1L);
            y1L = lpSample;
            bufferL[index] -= lpSample;

            lpSample = (Sample)(bufferR[index] * b0 - a1 * y1R);
            y1R = lpSample;
            bufferR[index] -= lpSample;
        }
    }
    
    double b0 = 1.0;
    double a1 = 0.0;
    Sample y1L = 0.0f;
    Sample y1R = 0.0f;
};

#endif // ONEPOLE_H
