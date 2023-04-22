/*
  ==============================================================================

    IRLoader.h
    Created: 22 Apr 2023 5:34:02pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class IRLoader {

public:
  IRLoader() {};

  void prepareToPlay(juce::dsp::ProcessSpec &spec) {
    samplerate = spec.sampleRate;
  }

  void performConvolution(juce::AudioBuffer<float>* buffer);

private:

  float samplerate; 
  juce::dsp::Convolution irConvolver;


};