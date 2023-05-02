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

class AmpModelerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
	AmpModelerAudioProcessorEditor (AmpModelerAudioProcessor&);
	~AmpModelerAudioProcessorEditor() override;

	//==============================================================================
	void paint (juce::Graphics&) override;
	void resized() override;

private:


    juce::Slider bassEQSlider;
    juce::Slider midEQSlider;
    juce::Slider trebbleEQSlider;
    juce::Label bassEQSliderLabel {"BASS_EQ_SLIDER_LABEL", "Bass"};
    juce::Label midEQSliderLabel {"MIDDLE_EQ_SLIDER_LABEL", "Mid"};
    juce::Label trebbleEQSliderLabel {"TREBBLE_EQ_SLIDER_LABEL", "Trebble"};
    std::unique_ptr<SliderAttachment> bassEQSliderAttachment;
    std::unique_ptr<SliderAttachment> midEQSliderAttachment;
    std::unique_ptr<SliderAttachment> trebbleEQSliderAttachment;


    juce::Slider gainSlider;
    juce::Label gainSliderLabel;
    std::unique_ptr<SliderAttachment> gainSliderAttachment;

	juce::Slider volumeSlider;
    juce::Label volumeSliderLabel;
    std::unique_ptr<SliderAttachment> volumeSliderAttachment;


    juce::TextButton irLoadButton {"Load IR"};


    AmpModelerAudioProcessor& audioProcessor;

    void createKnob(juce::Slider &slider, juce::Label &label);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessorEditor)
};
