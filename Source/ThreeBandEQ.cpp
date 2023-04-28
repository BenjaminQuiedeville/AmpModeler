/*
  ==============================================================================

    ThreeBandEQ.cpp
    Created: 23 Apr 2023 9:08:03pm
    Author:  Benjamin

  ==============================================================================
*/

#include "ThreeBandEQ.h"

ThreeBandEQ::ThreeBandEQ() {

    bassFilter = std::make_unique<Biquad>(FilterTypes::PEAK);
    midFilter = std::make_unique<Biquad>(FilterTypes::PEAK);
    trebbleFilter = std::make_unique<Biquad>(FilterTypes::PEAK);

}

ThreeBandEQ::~ThreeBandEQ() {}

void ThreeBandEQ::prepareToPlay(juce::dsp::ProcessSpec &spec) {

    bassFilter->prepareToPlay(spec);
    midFilter->prepareToPlay(spec);
    trebbleFilter->prepareToPlay(spec);
}

void ThreeBandEQ::process(AudioBlock &audioBlock) {

    for (size_t channelIndex = 0; channelIndex < audioBlock.getNumChannels(); channelIndex++) {
        float *bufferPtr = audioBlock.getChannelPointer(channelIndex);

        for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {
            
            bufferPtr[index] = bassFilter->process(bufferPtr[index]);
            bufferPtr[index] = midFilter->process(bufferPtr[index]);
            bufferPtr[index] = trebbleFilter->process(bufferPtr[index]);
        }
    }

}

void ThreeBandEQ::updateGains(float bassGain, float midGain, float trebbleGain) {

    bassFilter->setCoefficients(bassFreq, bassQ, bassGain);
    midFilter->setCoefficients(midFreq, midQ, midGain);
    trebbleFilter->setCoefficients(trebbleFreq, trebbleQ, trebbleGain);
}
