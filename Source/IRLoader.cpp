/*
  ==============================================================================

    IRLoader.cpp
    Created: 22 Apr 2023 5:34:02pm
    Author:  Benjamin

  ==============================================================================
*/

#include "IRLoader.h"
#include "baseIR.inc"

IRLoader::IRLoader() {
    irConvolver = new juce::dsp::Convolution();

}

IRLoader::~IRLoader() {

    delete irConvolver;

}

void IRLoader::prepareToPlay(juce::dsp::ProcessSpec &spec) {

    samplerate = spec.sampleRate;

    irConvolver->prepare(spec);

    safetyGain.prepare(spec);
    safetyGain.setRampDurationSeconds(0.02f);
    safetyGain.setGainLinear(0.0f);

    loadDefaultIR();
}

void IRLoader::loadDefaultIR() {

    // juce::AudioBuffer<sample_t> baseIRBuffer(()baseIR, 1, 0, BASE_IR_SIZE);

    irConvolver->loadImpulseResponse(
        (void *)baseIR,
        BASE_IR_SIZE * sizeof(sample_t), 
        juce::dsp::Convolution::Stereo::no,
        juce::dsp::Convolution::Trim::no,
        BASE_IR_SIZE,
        juce::dsp::Convolution::Normalise::no
    );

    irConvolver->reset();
    safetyGain.setGainLinear(1.0f);
}

void IRLoader::loadIR() {

    juce::FileChooser irChooser { "Load an IR" };

    if (irChooser.browseForFileToOpen()) {
        const juce::File irFile = irChooser.getResult();
        irConvolver->loadImpulseResponse(
            irFile, 
            juce::dsp::Convolution::Stereo::no, 
            juce::dsp::Convolution::Trim::no, 
            0
        );

        irConvolver->reset();
        safetyGain.setGainLinear(1.0f);
    }
}

void IRLoader::performConvolution(AudioBlock& audioBlock) {

    irConvolver->process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    safetyGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}
