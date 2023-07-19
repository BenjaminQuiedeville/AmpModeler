/*
  ==============================================================================

    OnepoleFilter.h
    Created: 4 May 2023 8:22:11pm
    Author:  Benjamin

  ==============================================================================
*/
#include <JuceHeader.h>
#include "types.h"

#pragma once

struct OnepoleFilter {

    OnepoleFilter() {}

    void prepareToPlay(juce::dsp::ProcessSpec &spec) {
        piOverSamplerate = juce::MathConstants<float>::pi / spec.sampleRate;
    }

    void setCoefficients(const float frequency) {
        b0 = sinf(piOverSamplerate * frequency);
        a1 = b0 - 1.0f;
    }

    sample_t processLowPass(sample_t sample) {
        sample = sample * b0 - a1 * y1;
        y1 = sample;
        return sample;
    }

    sample_t processHighPass(sample_t sample) {
        sample_t lpSample = sample * b0 - a1 * y1;
        y1 = lpSample;
        return sample - lpSample;
    }

    void processBufferLowpass(float *bufferPtr, size_t numSamples) {
        for (size_t index = 0; index < numSamples; index++) {
            bufferPtr[index] = processLowPass(bufferPtr[index]);
        }
    }

    void processBufferHighpass(float *bufferPtr, size_t numSamples) {
        for (size_t index = 0; index < numSamples; index++) {
            bufferPtr[index] = processHighPass(bufferPtr[index]);
        }
    }
    

    float piOverSamplerate;

    sample_t b0 = 1.0f;
    sample_t a1 = 0.0f;
    sample_t y1 = 0.0f;
};
