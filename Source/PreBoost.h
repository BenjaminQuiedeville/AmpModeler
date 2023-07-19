/*
  ==============================================================================

    preBoost.h
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "types.h"
#include "Biquad.h"

struct PreBoost {

    PreBoost();
    ~PreBoost();

    void prepareToPlay(juce::dsp::ProcessSpec &spec);
    void updateTight(const float newFrequency);
    void updateBite(const float newGain);
    void process(AudioBlock &audioBlock);


    float tightFrequency;
    const float biteFrequency = 1700.0f;

    float tightQ = 0.7f;
    float biteQ = 0.5f;

    float biteGain;

    Biquad *tightFilter;
    Biquad *biteFilter;

};