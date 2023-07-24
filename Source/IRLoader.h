/*
  ==============================================================================

    IRLoader.h
    Created: 22 Apr 2023 5:34:02pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once
#define JUCE_MODAL_LOOPS_PERMITTED 1

#include <JuceHeader.h>
#include "types.h"

struct IRLoader {

    IRLoader();

    void prepareToPlay(juce::dsp::ProcessSpec &spec);

    void loadIR();
    void loadDefaultIR();
    void performConvolution(AudioBlock& audioBlock);

    double samplerate;

    juce::AudioFormatManager irFormarManager;
    // juce::AudioFormatReader irFormatLoader;
    juce::AudioBuffer<sample_t> irBuffer;
    juce::dsp::Convolution *irConvolver;

    juce::dsp::Gain<sample_t> safetyGain;
};