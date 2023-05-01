/*
  ==============================================================================

    PreampDistorsion.h
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "types.h"

class PreampDistorsion {
public:
    PreampDistorsion();

    void prepareToPlay(juce::dsp::ProcessSpec &spec);
    void process(AudioBlock &audioblock);

    juce::dsp::Gain<sample_t> preGain;

private:

    float m_samplerate;
    float stageGain;

    juce::dsp::Gain<sample_t> postGain;
    juce::dsp::Oversampling<sample_t> overSampler;
};