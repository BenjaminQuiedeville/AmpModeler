/*
  ==============================================================================

    ThreeBandEQ.cpp
    Created: 23 Apr 2023 9:08:03pm
    Author:  Benjamin

  ==============================================================================
*/

#include "ThreeBandEQ.h"

ThreeBandEQ::ThreeBandEQ() {

    for (uint8_t i = 0; i < nChans; i ++) {
        bassFilters[i]->setFilterType(Biquad::FilterTypes::PEAK);
        midFilters[i]->setFilterType(Biquad::FilterTypes::PEAK);
        trebbleFilters[i]->setFilterType(Biquad::FilterTypes::PEAK);
    }

}

ThreeBandEQ::~ThreeBandEQ() {}

void ThreeBandEQ::prepareToPlay(juce::dsp::ProcessSpec &spec) {

    for (uint8_t i = 0; i < nChans; i ++) {
        bassFilters[i]->prepareToPlay(spec);
        midFilters[i]->prepareToPlay(spec);
        trebbleFilters[i]->prepareToPlay(spec);
    }
}

void ThreeBandEQ::process(AudioBlock &audioBlock) {


    for (uint8_t channel = 0; channel < nChans; channel++) {
        
        float *bufferPtr = audioBlock.getChannelPointer(channel);
        for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {
            
            bassFilters[channel]->process(&bufferPtr[index]);
            midFilters[channel]->process(&bufferPtr[index]);
            trebbleFilters[channel]->process(&bufferPtr[index]);
        }
    }
}

void ThreeBandEQ::updateGains(const float bassGain, const float midGain, const float trebbleGain) {

    for (uint8_t i = 0; i < nChans; i ++) {
        bassFilters[i]->setCoefficients(bassFreq, bassQ, bassGain);
        midFilters[i]->setCoefficients(midFreq, midQ, midGain);
        trebbleFilters[i]->setCoefficients(trebbleFreq, trebbleQ, trebbleGain);
    }
}
