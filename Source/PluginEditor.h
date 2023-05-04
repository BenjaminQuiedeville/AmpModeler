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
    juce::Label bassEQSliderLabel {"BASS_EQ_SLIDER_LABEL", "Bass"};
    std::unique_ptr<SliderAttachment> bassEQSliderAttachment;

    juce::Slider midEQSlider;
    juce::Label midEQSliderLabel {"MIDDLE_EQ_SLIDER_LABEL", "Mid"};
    std::unique_ptr<SliderAttachment> midEQSliderAttachment;

    juce::Slider trebbleEQSlider;
    juce::Label trebbleEQSliderLabel {"TREBBLE_EQ_SLIDER_LABEL", "Trebble"};
    std::unique_ptr<SliderAttachment> trebbleEQSliderAttachment;


    juce::Slider gainSlider;
    juce::Label gainSliderLabel {"GAIN_SLIDER_LABEL", "Pre Gain"};
    std::unique_ptr<SliderAttachment> gainSliderAttachment;

	juce::Slider volumeSlider;
    juce::Label volumeSliderLabel;
    std::unique_ptr<SliderAttachment> volumeSliderAttachment;


    juce::TextButton irLoadButton {"Load IR"};


    AmpModelerAudioProcessor& audioProcessor;

    void createKnob(juce::Slider &slider, juce::Label &label);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessorEditor)
};
