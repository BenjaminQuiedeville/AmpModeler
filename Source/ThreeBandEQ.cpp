/*
  ==============================================================================

    ThreeBandEQ.cpp
    Created: 23 Apr 2023 9:08:03pm
    Author:  Benjamin

  ==============================================================================
*/

#include "ThreeBandEQ.h"

ThreeBandEQ::ThreeBandEQ() {

    bassFilters    = new Biquad(Biquad::FilterType::PEAK);
    midFilters     = new Biquad(Biquad::FilterType::PEAK);
    trebbleFilters = new Biquad(Biquad::FilterType::PEAK);
}

ThreeBandEQ::~ThreeBandEQ() {
    delete bassFilters;
    delete midFilters;
    delete trebbleFilters;
}

void ThreeBandEQ::prepareToPlay(double _samplerate) {

    bassFilters->prepareToPlay(_samplerate);
    midFilters->prepareToPlay(_samplerate);
    trebbleFilters->prepareToPlay(_samplerate);
}

void ThreeBandEQ::process(AudioBlock &audioBlock) {

    sample_t *bufferPtr = audioBlock.getChannelPointer(0);
    size_t nSamples = audioBlock.getNumSamples();

    bassFilters->process(bufferPtr, nSamples);
    midFilters->process(bufferPtr, nSamples);
    trebbleFilters->process(bufferPtr, nSamples);
}

void ThreeBandEQ::updateGains(double bassGain, double midGain, double trebbleGain) {

    bassFilters->setCoefficients(bassFreq, bassQ, bassGain);
    midFilters->setCoefficients(midFreq, midQ, midGain);
    trebbleFilters->setCoefficients(trebbleFreq, trebbleQ, trebbleGain);
}
