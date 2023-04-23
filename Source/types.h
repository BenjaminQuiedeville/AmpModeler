/*
  ==============================================================================

    types.h
    Created: 23 Apr 2023 11:32:09am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

typedef float sample_t;
using AudioBlock = juce::dsp::AudioBlock<sample_t>;

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;


