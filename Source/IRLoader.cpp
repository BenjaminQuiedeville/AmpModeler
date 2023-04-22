/*
  ==============================================================================

    IRLoader.cpp
    Created: 22 Apr 2023 5:34:02pm
    Author:  Benjamin

  ==============================================================================
*/

#include "IRLoader.h"

IRLoader::IRLoader() {

}



void IRLoader::prepareToPlay(juce::dsp::ProcessSpec &spec) {

    irConvolver.prepare(spec);

    safetyGain.prepare(spec);
    safetyGain.setRampDurationSeconds(0.02f);
    safetyGain.setGainLinear(0.0f);

    loadDefaultIR();
}

void IRLoader::loadDefaultIR() {

    const juce::File irFile = juce::File(filepath);

    irConvolver.loadImpulseResponse(
        irFile, 
        juce::dsp::Convolution::Stereo::no, 
        juce::dsp::Convolution::Trim::no, 
        0
    );

    irConvolver.reset();
    safetyGain.setGainLinear(1.0f);
}

void IRLoader::loadIR() {

    juce::FileChooser irChooser { "Load an IR" };

    if (irChooser.browseForFileToOpen()) {
        const juce::File irFile = irChooser.getResult();
        irConvolver.loadImpulseResponse(
            irFile, 
            juce::dsp::Convolution::Stereo::no, 
            juce::dsp::Convolution::Trim::no, 
            0
        );

        irConvolver.reset();
        safetyGain.setGainLinear(1.0f);
    }
}

void IRLoader::performConvolution(juce::dsp::AudioBlock<float>& audioBlock) {

    irConvolver.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    safetyGain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
}
