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

class IRLoader {

public:
    IRLoader();

    void prepareToPlay(juce::dsp::ProcessSpec &spec);

    void loadIR();
    void loadDefaultIR();
    void performConvolution(juce::dsp::AudioBlock<float>& audioBlock);

private:

    const std::string filepath = "D:/Documents/Projets musique/ImpulseResponses/Rainbows/44.1/01 Modern Standard.wav";

    juce::AudioBuffer<float> irBuffer;
    juce::dsp::Convolution irConvolver;

    juce::dsp::Gain<float> safetyGain;
};