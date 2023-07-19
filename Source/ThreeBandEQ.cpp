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

void ThreeBandEQ::prepareToPlay(juce::dsp::ProcessSpec &spec) {

    bassFilters->prepareToPlay(spec);
    midFilters->prepareToPlay(spec);
    trebbleFilters->prepareToPlay(spec);
}

void ThreeBandEQ::process(AudioBlock &audioBlock) {

    float *bufferPtr = audioBlock.getChannelPointer(0);
    for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {
        
        bassFilters->process(&bufferPtr[index]);
        midFilters->process(&bufferPtr[index]);
        trebbleFilters->process(&bufferPtr[index]);
    }
}

void ThreeBandEQ::updateGains(const float bassGain, const float midGain, const float trebbleGain) {

    bassFilters->setCoefficients(bassFreq, bassQ, bassGain);
    midFilters->setCoefficients(midFreq, midQ, midGain);
    trebbleFilters->setCoefficients(trebbleFreq, trebbleQ, trebbleGain);
}
