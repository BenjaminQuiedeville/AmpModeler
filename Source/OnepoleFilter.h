/*
  ==============================================================================

    OnepoleFilter.h
    Created: 4 May 2023 8:22:11pm
    Author:  Benjamin

  ==============================================================================
*/

#ifndef ONEPOLE_H
#define ONEPOLE_H

#include <JuceHeader.h>
#include "common.h"


struct OnepoleFilter {

    OnepoleFilter() {}

    void prepareToPlay(double _samplerate) {
        piOverSamplerate = juce::MathConstants<float>::pi / _samplerate;
    }

    void setCoefficients(double frequency) {
        b0 = sin(piOverSamplerate * frequency);
        a1 = b0 - 1.0f;
    }

    inline sample_t processLowPass(sample_t sample) {
        sample = sample * b0 - a1 * y1;
        y1 = sample;
        return sample;
    }

    inline sample_t processHighPass(sample_t sample) {
        sample_t lpSample = sample * b0 - a1 * y1;
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
    

    double piOverSamplerate;

    double b0 = 1.0;
    double a1 = 0.0;
    sample_t y1 = 0.0;
};

#endif // ONEPOLE_H
