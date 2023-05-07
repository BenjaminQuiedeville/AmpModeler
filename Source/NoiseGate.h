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

    float threshold;
private:

    float samplerate;
    size_t windowSize; //10ms

    float currentRms;
    juce::AudioBuffer<sample_t> rmsBuffer;
    float *rmsBufferPtr;
    size_t rmsBufferIndex;

    float attackSeconds = 1.0f * 0.001;
    float releaseSeconds = 10.0f * 0.001;

    bool isOpen;
    juce::dsp::Gain<sample_t> gateGainLeft;
    juce::dsp::Gain<sample_t> gateGainRight;

    float computeRampIncrement(float currentValue, float targetValue, float nSamples);
    float rampStep(float currentValue, float targetValue, float increment);

};