/*
  ==============================================================================

    preBoost.cpp
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#include "Boost.h"

Boost::Boost() {
    tightFilter = new OnepoleFilter();
    biteFilter = new Biquad(FilterType::BIQUAD_PEAK);
}

Boost::~Boost() {
    delete tightFilter;
    delete biteFilter;

}

void Boost::prepareToPlay(double _samplerate) {
    tightFilter->prepareToPlay(_samplerate);
    biteFilter->prepareToPlay(_samplerate);
}

void Boost::updateTight(const float newFrequency) {
    tightFilter->setCoefficients(newFrequency);
}

void Boost::updateBite(const float newGain) {
    biteFilter->setCoefficients(biteFrequency, biteQ, newGain);
}

