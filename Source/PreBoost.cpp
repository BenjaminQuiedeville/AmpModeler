/*
  ==============================================================================

    preBoost.cpp
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreBoost.h"

PreBoost::PreBoost() {
    tightFilter = new Biquad(FilterType::BIQUAD_LOWPASS);
    biteFilter = new Biquad(FilterType::BIQUAD_PEAK);
}

PreBoost::~PreBoost() {
    delete tightFilter;
    delete biteFilter;

}

void PreBoost::prepareToPlay(double _samplerate) {
    tightFilter->prepareToPlay(_samplerate);
    biteFilter->prepareToPlay(_samplerate);
}

void PreBoost::updateTight(const float newFrequency) {
    tightFilter->setCoefficients(newFrequency, tightQ, 1.0f);
}

void PreBoost::updateBite(const float newGain) {
    biteFilter->setCoefficients(biteFrequency, biteQ, newGain);
}

void PreBoost::process(AudioBlock &audioBlock) {

    float *bufferPtr = audioBlock.getChannelPointer(0);
    size_t nSamples = audioBlock.getNumSamples();

    tightFilter->process(bufferPtr, nSamples);
    biteFilter->process(bufferPtr, nSamples);

}
