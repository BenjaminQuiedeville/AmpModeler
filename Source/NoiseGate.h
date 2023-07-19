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

struct NoiseGate {
    NoiseGate() {};

    void prepareToPlay(juce::dsp::ProcessSpec &spec);
    void process(AudioBlock &audioBlock);

    float threshold;

    float samplerate;
    size_t windowSize; //10ms

    float currentRms;
    juce::AudioBuffer<sample_t> rmsBuffer;
    float *rmsBufferPtr;
    size_t rmsBufferIndex;

    float attackTimeSeconds = 0.1f * 0.001;
    float releaseTimeSeconds = 15.0f * 0.001;

    bool isOpen;
    juce::dsp::Gain<sample_t> gateGain;
};