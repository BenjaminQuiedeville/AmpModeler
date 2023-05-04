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

class OnepoleFilter {
public:

    void prepareToPlay(juce::dsp::ProcessSpec &spec) {
        piOverSamplerate = juce::MathConstants<float>::pi / spec.sampleRate;
    }

    void setCoefficients(const float frequency) {
        b0 = sinf(piOverSamplerate * frequency);
        a1 = b0 - 1.0f;
    }

    void init() {
        b0 = 1.0f;
        a1 = 0.0f;
        y1 = 0.0f;
    }

    void processLowPass(sample_t *sample) {
        *sample = *sample * b0 - a1 * y1;
        y1 = *sample;
    }

    void processHighPass(sample_t *sample) {
        float tempSample = *sample * b0 - y1 * a1;
        y1 = tempSample;
        *sample = 1.0f - tempSample;
    }

private:

    float piOverSamplerate;

    sample_t b0 = 1.0f;
    sample_t a1 = 0.0f;
    sample_t y1 = 0.0f;
};
