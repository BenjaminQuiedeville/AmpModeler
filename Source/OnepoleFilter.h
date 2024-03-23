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
        y1 = 0.0f;
    }

    void setCoefficients(double frequency, double samplerate) {
        b0 = sin(juce::MathConstants<float>::pi / samplerate * frequency);
        a1 = b0 - 1.0;
    }

    inline sample_t processLowPass(sample_t sample) {
        sample = (sample_t)(sample * b0 - a1 * y1);
        y1 = sample;
        return sample;
    }

    inline sample_t processHighPass(sample_t sample) {
        sample_t lpSample = (sample_t)(sample * b0 - a1 * y1);
        y1 = lpSample;
        return sample - lpSample;
    }

    void processBufferLowpass(sample_t *buffer, size_t numSamples) {
        for (size_t index = 0; index < numSamples; index++) {
            buffer[index] = processLowPass(buffer[index]);
        }
    }

    void processBufferHighpass(sample_t *buffer, size_t numSamples) {
        for (size_t index = 0; index < numSamples; index++) {
            buffer[index] = processHighPass(buffer[index]);
        }
    }
    
    double b0 = 1.0;
    double a1 = 0.0;
    sample_t y1 = 0.0;
};

#endif // ONEPOLE_H
