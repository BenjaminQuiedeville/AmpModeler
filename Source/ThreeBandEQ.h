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


class ThreeBandEQ {
public:

    ThreeBandEQ();
    ~ThreeBandEQ();

    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void process(AudioBlock& audioBlock);
    void updateGains(float bassGain, float midGain, float trebbleGain);

private:

    Biquad *bassFilter;
    Biquad *midFilter;
    Biquad *trebbleFilter;


    float bassFreq = 120.0f;
    float midFreq = 750.0f;
    float trebbleFreq = 4500.0f;
    
    float bassQ = 0.5;
    float midQ = 0.5;
    float trebbleQ = 0.5;
};