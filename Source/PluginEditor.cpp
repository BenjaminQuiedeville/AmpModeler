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

    bassEQSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    bassEQSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    addAndMakeVisible(bassEQSlider);
    bassEQSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "3_BAND_EQ_BASS", bassEQSlider);

    bassEQSliderLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    bassEQSliderLabel.setJustificationType(juce::Justification::centred);
    bassEQSliderLabel.setFont(15.0f);
    addAndMakeVisible(bassEQSliderLabel);


    middleEQSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    middleEQSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    addAndMakeVisible(middleEQSlider);
    bassEQSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "3_BAND_EQ_MIDDLE", middleEQSlider);

    middleEQSliderLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    middleEQSliderLabel.setJustificationType(juce::Justification::centred);
    middleEQSliderLabel.setFont(15.0f);
    addAndMakeVisible(middleEQSliderLabel);    
    

    trebbleEQSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    trebbleEQSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    addAndMakeVisible(trebbleEQSlider);
    bassEQSliderAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "3_BAND_EQ_TREBBLE", trebbleEQSlider);

    trebbleEQSliderLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    trebbleEQSliderLabel.setJustificationType(juce::Justification::centred);
    trebbleEQSliderLabel.setFont(15.0f);
    addAndMakeVisible(trebbleEQSliderLabel);




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
    middleEQSlider.setBounds(getWidth()/2 - knobSize/2, getHeight()/2 - knobSize/2, knobSize, knobSize);
    middleEQSliderLabel.setBounds(middleEQSlider.getX(), middleEQSlider.getY() - 20, middleEQSlider.getWidth(), 20);
    trebbleEQSlider.setBounds(getWidth()/2 + knobSize/2, getHeight()/2 - knobSize/2, knobSize, knobSize);
    trebbleEQSliderLabel.setBounds(trebbleEQSlider.getX(), trebbleEQSlider.getY() - 20, trebbleEQSlider.getWidth(), 20);

	volumeSlider.setBounds(getWidth()/12 * 11 - horizontalMargin, getHeight()/2 + 2*knobSize, knobSize, knobSize);
    irLoadButton.setBounds(getWidth()/12* 11 - horizontalMargin, getHeight()/2 - 100, 100, 50);

}
