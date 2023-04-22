/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

class AmpModelerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
	AmpModelerAudioProcessorEditor (AmpModelerAudioProcessor&);
	~AmpModelerAudioProcessorEditor() override;

	//==============================================================================
	void paint (juce::Graphics&) override;
	void resized() override;

private:


	juce::Slider volumeSlider;
    std::unique_ptr<SliderAttachment> volumeSliderAttachment;

    juce::TextButton irLoadButton {"Load IR"};


    AmpModelerAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessorEditor)
};
