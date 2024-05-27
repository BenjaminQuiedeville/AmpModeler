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
        b0 = sin(juce::MathConstants<double>::pi / samplerate * frequency);
        a1 = b0 - 1.0;
    }

    inline Sample processLowpass(Sample sample) {
        sample = (Sample)(sample * b0 - a1 * y1);
        y1 = sample;
        return sample;
    }

    inline Sample processHighpass(Sample sample) {
        Sample lpSample = (Sample)(sample * b0 - a1 * y1);
        y1 = lpSample;
        return sample - lpSample;
    }

    void processBufferLowpass(Sample *buffer, size_t numSamples) {
        for (size_t index = 0; index < numSamples; index++) {
            buffer[index] = processLowpass(buffer[index]);
        }
    }

    void processBufferHighpass(Sample *buffer, size_t numSamples) {
        for (size_t index = 0; index < numSamples; index++) {
            buffer[index] = processHighpass(buffer[index]);
        }
    }
    
    double b0 = 1.0;
    double a1 = 0.0;
    Sample y1 = 0.0;
};

#endif // ONEPOLE_H
