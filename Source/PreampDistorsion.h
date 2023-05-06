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
#include "OnepoleFilter.h"
#include <array>

enum DriveType {
    APPROX, 
    TANH,
    HARDCLIP,
};

class PreampDistorsion {
public:
    PreampDistorsion();

    void prepareToPlay(juce::dsp::ProcessSpec &spec);
    void process(AudioBlock &audioblock);

    juce::dsp::Gain<sample_t> preGain;
    juce::dsp::Gain<sample_t> postGain;

    DriveType driveType;

private:

    float m_samplerate;
    float stageGain;
    const float inputFilterFrequency = 900.0f;

    std::array<OnepoleFilter, 2> inputFilters;
    std::unique_ptr<juce::dsp::Oversampling<sample_t>> overSampler;

    sample_t processDrive(sample_t &sample, DriveType curveType);

    sample_t expappr(sample_t x) { 

        const sample_t x2 = x*x;
        const sample_t x3 = x2*x;
        const sample_t x4 = x3*x;
        const sample_t x5 = x4*x;
        return 1.0f + x + x2/2.0f + x3/6.0f + x4/24.0f + x5/120.0f; 
    }

    sample_t tanhApprox(sample_t x) {
        return (expappr(x) - expappr(-x))/(expappr(x) + expappr(-x)); 
    }

};