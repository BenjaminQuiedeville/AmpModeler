/*
  ==============================================================================

    Biquad.h
    Created: 23 Apr 2023 12:01:44am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "types.h"

enum FilterTypes {
    LOWPASS = 0,
    HIGHPASS,
    PEAK,
    LOWSHELF,
    HIGHSHELF,
    NFILTERTYPES,
};

class Biquad {

public:

    Biquad() {};

    void setFilterType(FilterTypes type) { filterType = type; }

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void reset();
    void setCoefficients(const float frequency, const float Q, const float gaindB);
    void process(sample_t *sample);

private:

    FilterTypes filterType;
    double samplerate;
    double twoPiOverSamplerate;
    sample_t b0, b1, b2, a1, a2;

    sample_t x1, x2, y1, y2;


};