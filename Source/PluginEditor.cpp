/*
	==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

	==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpModelerAudioProcessorEditor::AmpModelerAudioProcessorEditor (AmpModelerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    
	volumeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
	volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
	addAndMakeVisible(volumeSlider);
    volumeSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "MASTER_VOLUME", volumeSlider);

    irLoadButton.onClick = [&]() {};

    setSize (600, 400);
}

AmpModelerAudioProcessorEditor::~AmpModelerAudioProcessorEditor()
{
}

//==============================================================================
void AmpModelerAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);
}

void AmpModelerAudioProcessorEditor::resized()
{

	volumeSlider.setBounds(getWidth()/2 - 200, getHeight()/2 - 100, 400, 200);

}
