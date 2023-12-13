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
    level = new SmoothParam();
}

Boost::~Boost() {
    delete tightFilter;
    delete biteFilter;
    delete level;

}

void Boost::prepareToPlay(double _samplerate) {
    tightFilter->prepareToPlay(_samplerate);
    biteFilter->prepareToPlay(_samplerate);
    level->init(_samplerate, 0.02, 0.0, CurveType::SMOOTH_PARAM_LIN);
}

void Boost::updateTight(const float newFrequency) {
    tightFilter->setCoefficients(newFrequency);
}

void Boost::updateBite(const float newGain) {
    biteFilter->setCoefficients(BOOST_BITE_FREQ, BOOST_BITE_Q, newGain);
}

