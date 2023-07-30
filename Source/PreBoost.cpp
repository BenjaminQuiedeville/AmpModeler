/*
  ==============================================================================

    preBoost.cpp
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreBoost.h"

PreBoost::PreBoost() {
    tightFilter = new Biquad(Biquad::FilterType::LOWPASS);
    biteFilter = new Biquad(Biquad::FilterType::PEAK);
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
    for (size_t i = 0; i < audioBlock.getNumSamples(); i++) {

        tightFilter->process(&bufferPtr[i]);
        biteFilter->process(&bufferPtr[i]);

    }

}
