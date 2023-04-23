/*
  ==============================================================================

    Biquad.h
    Created: 23 Apr 2023 12:01:44am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

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

    Biquad(FilterTypes type) {
        filterType = type;
    };

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void reset();
    void setCoefficient(float frequency, float Q, float gaindB);
    float process(float& sample);

private:

    FilterTypes filterType;
    float samplerate;
    float twoPiOverSampelrate;
    float b0, b1, b2, a1, a2;

    float x1, x2, y1, y2;


};