/*
==============================================================================

    Biquad.h
    Created: 23 Apr 2023 12:01:44am
    Author:  Benjamin

==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "common.h"


struct Biquad {

    enum FilterType {
        LOWPASS = 0,
        HIGHPASS,
        PEAK,
        LOWSHELF,
        HIGHSHELF,
        NFILTERTYPES,
    };

    Biquad(FilterType type) { filterType = type; }

    void prepareToPlay(double _samplerate);
    void reset();
    void setCoefficients(double frequency, double Q, double gaindB);

    void process(sample_t *signal, size_t nSamples) {

        for (size_t i = 0; i < nSamples; i++) {
            sample_t outputSample = signal[i]*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
            x2 = x1;
            x1 = signal[i];
            y2 = y1;
            y1 = outputSample;
            signal[i] = outputSample;
        }
    }

    inline void processSample(sample_t *sample) {

        sample_t outputSample = *sample*b0 + x1*b1 + x2*b2 - y1*a1 - y2*a2;
        x2 = x1;
        x1 = *sample;
        y2 = y1;
        y1 = outputSample;
        *sample = outputSample;
    }

    FilterType filterType;
    double samplerate;
    double twoPiOverSamplerate;
    double b0, b1, b2, a1, a2;

    sample_t x1, x2, y1, y2;


};
