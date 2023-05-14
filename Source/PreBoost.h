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

class PreBoost {
public:

    PreBoost();

    void prepareToPlay(juce::dsp::ProcessSpec &spec);
    void updateTight(const float newFrequency);
    void updateBite(const float newGain);
    void process(AudioBlock &audioBlock);



private:

    float tightFrequency;
    const float biteFrequency = 1700.0f;

    float tightQ = 0.7f;
    float biteQ = 0.5f;

    float biteGain;

    std::unique_ptr<Biquad> tightFilter;
    std::unique_ptr<Biquad> biteFilter;

};