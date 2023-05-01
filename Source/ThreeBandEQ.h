/*
  ==============================================================================

    ThreeBandEQ.h
    Created: 23 Apr 2023 9:08:03pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Biquad.h"
#include "types.h"
#include "array"


class ThreeBandEQ {
public:

    ThreeBandEQ();
    ~ThreeBandEQ();

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(AudioBlock& audioBlock);
    void updateGains(const float bassGain, const float midGain, const float trebbleGain);

private:

    const uint8_t nChans = 2;
    // const uint8_t nBands = 3;

    std::unique_ptr<Biquad[]> bassFilters;
    std::unique_ptr<Biquad[]> midFilters;
    std::unique_ptr<Biquad[]> trebbleFilters;


    const float bassFreq = 120.0f;
    const float midFreq = 750.0f;
    const float trebbleFreq = 4500.0f;
    
    const float bassQ = 0.5;
    const float midQ = 0.5;
    const float trebbleQ = 0.5;
};