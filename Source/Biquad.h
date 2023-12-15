/*
==============================================================================

    Biquad.h
    Created: 23 Apr 2023 12:01:44am
    Author:  Benjamin

==============================================================================
*/

#ifndef BIQUAD_H
#define BIQUAD_H

#include "JuceHeader.h"
#include "common.h"

enum FilterType {
    BIQUAD_LOWPASS = 0,
    BIQUAD_HIGHPASS,
    BIQUAD_PEAK,
    BIQUAD_LOWSHELF,
    BIQUAD_HIGHSHELF,
    BIQUAD_NFILTERTYPES,
};


struct Biquad {

    Biquad(FilterType type) { filterType = (uint8_t)type; }

    void prepareToPlay() {

        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
        a1 = 0.0;
        a2 = 0.0;
        reset();
    }

    void reset() {
        x1 = 0.0f;
        x2 = 0.0f;
        y1 = 0.0f;
        y2 = 0.0f;
    }

    void setCoefficients(double frequency, double Q, double gaindB, double samplerate);

    void processBuffer(sample_t *signal, size_t nSamples) {

        for (size_t i = 0; i < nSamples; i++) {
            signal[i] = process(signal[i]);
        }
    }

    inline sample_t process(sample_t sample) {

        sample_t outputSample = (float)(sample * b0 + x1 * b1 + x2 * b2
                              - y1 * a1 - y2 * a2);
        x2 = x1;
        x1 = sample;
        y2 = y1;
        y1 = outputSample;
        return outputSample;
    }

    double b0 = 1.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double a1 = 0.0;
    double a2 = 0.0;

    sample_t x1 = 0.0f;
    sample_t x2 = 0.0f;
    sample_t y1 = 0.0f;
    sample_t y2 = 0.0f;
    uint8_t filterType;

};


#endif // BIQUAD_H
