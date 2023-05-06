#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpModelerAudioProcessorEditor::AmpModelerAudioProcessorEditor (AmpModelerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    createKnob(bassEQSlider, bassEQSliderLabel, bassEQSliderAttachment, "3_BAND_EQ_BASS");
    createKnob(midEQSlider, midEQSliderLabel, midEQSliderAttachment, "3_BAND_EQ_MIDDLE");
    createKnob(trebbleEQSlider, trebbleEQSliderLabel, trebbleEQSliderAttachment, "3_BAND_EQ_TREBBLE");

    createKnob(gainSlider, gainSliderLabel, gainSliderAttachment, "PREAMP_GAIN");

    createKnob(volumeSlider, volumeSliderLabel, volumeSliderAttachment, "MASTER_VOLUME");

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
    
    const int knobSize = 100;
    const int horizontalMargin = 25;
    const int verticalMargin = 50;

    const int HEIGHT = getHeight() - 2*verticalMargin;
    const int WIDTH = getWidth() - 2*horizontalMargin;

    const int nRows = 4;
    const int nCols = 6;

    auto computeXcoord = [&](int col){ return horizontalMargin + WIDTH/nCols * col; };
    auto computeYcoord = [&](int row){ return verticalMargin + HEIGHT/nRows * row; };


    gainSlider.setBounds(computeXcoord(1), computeYcoord(1), knobSize, knobSize);
    gainSliderLabel.setBounds(gainSlider.getX(), gainSlider.getY() - 20, gainSlider.getWidth(), 20);
    
    bassEQSlider.setBounds(computeXcoord(2), computeYcoord(1), knobSize, knobSize);
    bassEQSliderLabel.setBounds(bassEQSlider.getX(), bassEQSlider.getY() - 20, bassEQSlider.getWidth(), 20);

    midEQSlider.setBounds(computeXcoord(3), computeYcoord(1), knobSize, knobSize);
    midEQSliderLabel.setBounds(midEQSlider.getX(), midEQSlider.getY() - 20, midEQSlider.getWidth(), 20);
    
    trebbleEQSlider.setBounds(computeXcoord(4), computeYcoord(1), knobSize, knobSize);
    trebbleEQSliderLabel.setBounds(trebbleEQSlider.getX(), trebbleEQSlider.getY() - 20, trebbleEQSlider.getWidth(), 20);

	volumeSlider.setBounds(computeXcoord(5), computeYcoord(1), knobSize, knobSize);
    volumeSliderLabel.setBounds(volumeSlider.getX(), volumeSlider.getY() - 20, volumeSlider.getWidth(), 20);

    irLoadButton.setBounds(computeXcoord(5), computeYcoord(0), 100, 50);

}


void AmpModelerAudioProcessorEditor::createKnob(juce::Slider &slider, 
                                                juce::Label &label, 
                                                std::unique_ptr<SliderAttachment> &sliderAttachmnt, 
                                                const std::string paramID)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    addAndMakeVisible(slider);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    addAndMakeVisible(label);

    sliderAttachmnt = std::make_unique<SliderAttachment>(audioProcessor.apvts, paramID, slider);
}
