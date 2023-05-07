/*
  ==============================================================================

    NoiseGate.cpp
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

  ==============================================================================
*/

#include "NoiseGate.h"


void NoiseGate::prepareToPlay(juce::dsp::ProcessSpec &spec) {

    samplerate = spec.sampleRate;
    windowSize = floor(samplerate*0.001);

    attack = floor(samplerate * 0.001 * 1.0);
    release = floor(samplerate * 0.001 * 15.0);

    currentRms = 0.0f;
    threshold = -70.0f;

    gateGain.prepare(spec);
    gateGain.setRampDurationSeconds(0.0f);
}

void NoiseGate::setThreshold(const float newThreshold) {
    // threshold = pow(10, newThreshold/20.0f);
    threshold = juce::Decibels::decibelsToGain(newThreshold);
}


void NoiseGate::process(AudioBlock &audioBlock) {

}