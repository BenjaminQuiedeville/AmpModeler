/*
==============================================================================

    Biquad.h
    Created: 23 Apr 2023 12:01:44am
    Author:  Benjamin

==============================================================================
*/

#ifndef BIQUAD_H
#define BIQUAD_H

#include <JuceHeader.h>
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

    Biquad(FilterType type) { filterType = type; }

    void prepareToPlay(double _samplerate);
    void reset();
    void setCoefficients(double frequency, double Q, double gaindB);

    void Biquad::process(sample_t *signal, size_t nSamples) {

        for (size_t i = 0; i < nSamples; i++) {
            signal[i] = processSample(signal[i]);
        }
    }

    inline sample_t processSample(sample_t sample) {

        sample_t outputSample = sample*b0 
                            + x1*b1 
                            + x2*b2 
                            - y1*a1 
                            - y2*a2;
        x2 = x1;
        x1 = sample;
        y2 = y1;
        y1 = outputSample;
        return outputSample;
    }

    uint8_t filterType;
    double samplerate;
    double twoPiOverSamplerate;
    double b0, b1, b2, a1, a2;

    sample_t x1, x2, y1, y2;

};


#endif // BIQUAD_H
