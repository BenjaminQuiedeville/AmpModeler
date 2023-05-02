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

    createKnob(bassEQSlider, bassEQSliderLabel);
    bassEQSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "3_BAND_EQ_BASS", bassEQSlider);

    createKnob(midEQSlider, midEQSliderLabel);
    midEQSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "3_BAND_EQ_MIDDLE", midEQSlider);

    createKnob(trebbleEQSlider, trebbleEQSliderLabel);
    trebbleEQSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "3_BAND_EQ_TREBBLE", trebbleEQSlider);

    createKnob(gainSlider, gainSliderLabel);
    gainSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "PREAMP_GAIN", gainSlider);


	volumeSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	volumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
	addAndMakeVisible(volumeSlider);
    volumeSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "MASTER_VOLUME", volumeSlider);

    irLoadButton.onClick = [&]() { audioProcessor.irLoader.loadIR(); };
    addAndMakeVisible(irLoadButton);

    setSize (1000, 400);
}

AmpModelerAudioProcessorEditor::~AmpModelerAudioProcessorEditor()
{
}

//==============================================================================
void AmpModelerAudioProcessorEditor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);
}

void AmpModelerAudioProcessorEditor::resized() {

    int knobSize = 100;
    int horizontalMargin = 25;
    int verticalMargin = 50;

    bassEQSlider.setBounds(getWidth()/2 - knobSize/2*3, getHeight()/2 - knobSize/2, knobSize, knobSize);
    bassEQSliderLabel.setBounds(bassEQSlider.getX(), bassEQSlider.getY() - 20, bassEQSlider.getWidth(), 20);

    midEQSlider.setBounds(getWidth()/2 - knobSize/2, getHeight()/2 - knobSize/2, knobSize, knobSize);
    midEQSliderLabel.setBounds(midEQSlider.getX(), midEQSlider.getY() - 20, midEQSlider.getWidth(), 20);
    
    trebbleEQSlider.setBounds(getWidth()/2 + knobSize/2, getHeight()/2 - knobSize/2, knobSize, knobSize);
    trebbleEQSliderLabel.setBounds(trebbleEQSlider.getX(), trebbleEQSlider.getY() - 20, trebbleEQSlider.getWidth(), 20);

    gainSlider.setBounds(0, bassEQSlider.getY(), knobSize, knobSize);

	volumeSlider.setBounds(getWidth()/12 * 11 - horizontalMargin, getHeight()/2 + 2*knobSize, knobSize, knobSize);
    irLoadButton.setBounds(getWidth()/12* 11 - horizontalMargin, getHeight()/2 - 100, 100, 50);

}


void AmpModelerAudioProcessorEditor::createKnob(juce::Slider &slider, juce::Label &label) {
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    addAndMakeVisible(slider);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    addAndMakeVisible(label);
}
