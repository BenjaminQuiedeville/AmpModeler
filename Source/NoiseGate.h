/*
  ==============================================================================

    NoiseGate.h
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "types.h"

class NoiseGate {
public:

    NoiseGate() {};

    void prepareToPlay(juce::dsp::ProcessSpec &spec);
    void process(AudioBlock &audioBlock);
    void setThreshold(const float newThreshold);

private:

    float samplerate;
    float threshold;
    size_t windowSize; //10ms

    float currentRms;

    uint32_t attack;
    uint32_t release;

    juce::dsp::Gain<sample_t> gateGain;

};