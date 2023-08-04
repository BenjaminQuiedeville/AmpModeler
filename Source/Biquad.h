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
    void setCoefficients(const float frequency, const float Q, const float gaindB);

    inline void process(sample_t *sample) {

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
    sample_t b0, b1, b2, a1, a2;

    sample_t x1, x2, y1, y2;


};
