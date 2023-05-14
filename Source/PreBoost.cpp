/*
  ==============================================================================

    preBoost.cpp
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreBoost.h"

PreBoost::PreBoost() {
    tightFilter = std::make_unique<Biquad>(Biquad::FilterType::LOWPASS);
    biteFilter = std::make_unique<Biquad>(Biquad::FilterType::PEAK);
}

void PreBoost::prepareToPlay(juce::dsp::ProcessSpec &spec) {
    tightFilter->prepareToPlay(spec);
    biteFilter->prepareToPlay(spec);
}

void PreBoost::updateTight(const float newFrequency) {
    tightFilter->setCoefficients(newFrequency, tightQ, 1.0f);
}

void PreBoost::updateBite(const float newGain) {
    biteFilter->setCoefficients(biteFrequency, biteQ, newGain);
}

void PreBoost::process(AudioBlock &audioBlock) {

    float *bufferPtr = audioBlock.getChannelPointer(0);
    for (size_t i = 0; i < audioBlock.getNumSamples(); i++) {

        tightFilter->process(&bufferPtr[i]);
        biteFilter->process(&bufferPtr[i]);

    }

}
