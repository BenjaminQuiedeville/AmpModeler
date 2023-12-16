/*
  ==============================================================================

    NoiseGate.h
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "common.h"
#include "SmoothParam.h"

struct NoiseGate {
    NoiseGate() {};

    void prepareToPlay(double _samplerate);
    void process(sample_t *input, sample_t *sidechain, size_t nSamples);

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
    SmoothParamIIR gateGain;
};
