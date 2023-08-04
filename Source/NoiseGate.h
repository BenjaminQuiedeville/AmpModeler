/*
  ==============================================================================

    NoiseGate.h
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "common.h"
#include "SmoothParam.h"

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

    double attackTimeMs  = 0.1;
    double releaseTimeMs = 15.0;

    bool isOpen;
    SmoothParam gateGain;
};
