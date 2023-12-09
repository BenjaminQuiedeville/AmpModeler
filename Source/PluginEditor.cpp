#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpModelerAudioProcessorEditor::AmpModelerAudioProcessorEditor (AmpModelerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    gateKnob         = std::make_unique<Knob>("GATE_KNOB_LABEL", "Gate Thresh");
    boostTopKnob     = std::make_unique<Knob>("BOOST_TOP_KNOB_LABEL" , "Boost Top");
    boostTightKnob   = std::make_unique<Knob>("BOOST_TIGHT_KNOB_LABEL", "Boost Tight");
    
    gainKnob         = std::make_unique<Knob>("GAIN_KNOB_LABEL", "Pre Gain");
    inputFilterKnob  = std::make_unique<Knob>("INPUT_KNOB_FILTER_LABEL", "Input Filter");
    
    bassEQKnob       = std::make_unique<Knob>("BASS_EQ_KNOB_LABEL", "Bass");
    midEQKnob        = std::make_unique<Knob>("MIDDLE_EQ_KNOB_LABEL", "Mid");
    trebbleEQKnob    = std::make_unique<Knob>("TREBBLE_EQ_KNOB_LABEL", "Trebble");
    preampVolumeKnob = std::make_unique<Knob>("PREMP_VOLUME_KNOB_LABEL", "Post Gain");

    resonanceKnob    = std::make_unique<Knob>("RESONANCE_KNOB_LABEL", "Resonance");
    presenceKnob     = std::make_unique<Knob>("PRESENCE_KNOB_LABEL", "Presence");

    volumeKnob       = std::make_unique<Knob>("MASTER_VOLUME_KNOB_LABEL", "Output Level");


    // createKnob(gateSlider, gateSliderLabel, gateSliderAttachment, ParamsID[GATE_THRESH]);

    // createKnob(gainSlider, gainSliderLabel, gainSliderAttachment, ParamsID[PREAMP_GAIN]);

    // createKnob(inputFilterSlider, inputFilterSliderLabel, inputFilterSliderAttachment, ParamsID[INPUT_FILTER]);

    // createKnob(bassEQSlider, bassEQSliderLabel, bassEQSliderAttachment, ParamsID[TONESTACK_BASS]);
    // createKnob(midEQSlider, midEQSliderLabel, midEQSliderAttachment, ParamsID[TONESTACK_MIDDLE]);
    // createKnob(trebbleEQSlider, trebbleEQSliderLabel, trebbleEQSliderAttachment, ParamsID[TONESTACK_TREBBLE]);

    // createKnob(preampVolumeSlider, preampVolumeSliderLabel, preampVolumeSliderAttachment, ParamsID[PREAMP_VOLUME]);

    // createKnob(volumeSlider, volumeSliderLabel, volumeSliderAttachment, ParamsID[MASTER_VOLUME]);

    createKnob(gateKnob.get(), ParamsID[GATE_THRESH]);
    createKnob(boostTopKnob.get(), ParamsID[BITE]);
    createKnob(boostTightKnob.get(), ParamsID[TIGHT]);

    createKnob(gainKnob.get(), ParamsID[PREAMP_GAIN]);

    createKnob(inputFilterKnob.get(), ParamsID[INPUT_FILTER]);

    createKnob(bassEQKnob.get(), ParamsID[TONESTACK_BASS]);
    createKnob(midEQKnob.get(), ParamsID[TONESTACK_MIDDLE]);
    createKnob(trebbleEQKnob.get(), ParamsID[TONESTACK_TREBBLE]);

    createKnob(preampVolumeKnob.get(), ParamsID[PREAMP_VOLUME]);

    createKnob(resonanceKnob.get(), ParamsID[RESONANCE]);
    createKnob(presenceKnob.get(), ParamsID[PRESENCE]);

    createKnob(volumeKnob.get(), ParamsID[MASTER_VOLUME]);


    irLoadButton.onClick = [&]() { audioProcessor.irLoader->loadIR(); };
    addAndMakeVisible(irLoadButton);

    setSize (1200, 600);
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
    const int nCols = 8;

    auto computeXcoord = [&](int col) { return horizontalMargin + WIDTH/nCols * col; };
    auto computeYcoord = [&](int row) { return verticalMargin + HEIGHT/nRows * row; };

    gateKnob->slider.setBounds(computeXcoord(0), computeYcoord(0), knobSize, knobSize);
    gateKnob->label.setBounds(gateKnob->slider.getX(), 
                              gateKnob->slider.getY() - 20, 
                              gateKnob->slider.getWidth(), 
                              20);

    boostTopKnob->slider.setBounds(computeXcoord(0), computeYcoord(1), knobSize, knobSize);
    boostTopKnob->label.setBounds(boostTopKnob->slider.getX(),
                                  boostTopKnob->slider.getY() - 20,
                                  boostTopKnob->slider.getWidth(), 
                                  20);

    boostTightKnob->slider.setBounds(computeXcoord(0), computeYcoord(2), knobSize, knobSize);
    boostTightKnob->label.setBounds(boostTightKnob->slider.getX(),
                                    boostTightKnob->slider.getY() - 20,
                                    boostTightKnob->slider.getWidth(),
                                    20);

    gainKnob->slider.setBounds(computeXcoord(1), computeYcoord(1), knobSize, knobSize);
    gainKnob->label.setBounds(gainKnob->slider.getX(), 
                              gainKnob->slider.getY() - 20, 
                              gainKnob->slider.getWidth(), 
                              20);
    
    inputFilterKnob->slider.setBounds(computeXcoord(1), computeYcoord(2), knobSize, knobSize);
    inputFilterKnob->label.setBounds(inputFilterKnob->slider.getX(), 
                                     inputFilterKnob->slider.getY() - 20, 
                                     inputFilterKnob->slider.getWidth(), 
                                     20);

    bassEQKnob->slider.setBounds(computeXcoord(2), computeYcoord(1), knobSize, knobSize);
    bassEQKnob->label.setBounds(bassEQKnob->slider.getX(), 
                                bassEQKnob->slider.getY() - 20, 
                                bassEQKnob->slider.getWidth(), 
                                20);

    midEQKnob->slider.setBounds(computeXcoord(3), computeYcoord(1), knobSize, knobSize);
    midEQKnob->label.setBounds(midEQKnob->slider.getX(), 
                               midEQKnob->slider.getY() - 20, 
                               midEQKnob->slider.getWidth(), 
                               20);
    
    trebbleEQKnob->slider.setBounds(computeXcoord(4), computeYcoord(1), knobSize, knobSize);
    trebbleEQKnob->label.setBounds(trebbleEQKnob->slider.getX(), 
                                   trebbleEQKnob->slider.getY() - 20, 
                                   trebbleEQKnob->slider.getWidth(), 
                                   20);

    resonanceKnob->slider.setBounds(computeXcoord(5), computeYcoord(1), knobSize, knobSize);
    resonanceKnob->label.setBounds(resonanceKnob->slider.getX(),
                                   resonanceKnob->slider.getY() - 20,
                                   resonanceKnob->slider.getWidth(), 
                                   20);

    presenceKnob->slider.setBounds(computeXcoord(5), computeYcoord(2), knobSize, knobSize);
    presenceKnob->label.setBounds(presenceKnob->slider.getX(),
                                   presenceKnob->slider.getY() - 20,
                                   presenceKnob->slider.getWidth(), 
                                   20);

    

    preampVolumeKnob->slider.setBounds(computeXcoord(6), computeYcoord(1), knobSize, knobSize);
    preampVolumeKnob->label.setBounds(preampVolumeKnob->slider.getX(), 
                                      preampVolumeKnob->slider.getY() - 20, 
                                      preampVolumeKnob->slider.getWidth(), 
                                      20);




	volumeKnob->slider.setBounds(computeXcoord(6), computeYcoord(2), knobSize, knobSize);
    volumeKnob->label.setBounds(volumeKnob->slider.getX(), 
                                volumeKnob->slider.getY() - 20, 
                                volumeKnob->slider.getWidth(), 
                                20);

    irLoadButton.setBounds(computeXcoord(6), computeYcoord(0), 100, 50);

}


void AmpModelerAudioProcessorEditor::createKnob(Knob *knob, const juce::String& paramID) {

    knob->slider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    knob->slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    addAndMakeVisible(knob->slider);

    knob->label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    knob->label.setJustificationType(juce::Justification::centred);
    knob->label.setFont(15.0f);
    addAndMakeVisible(knob->label);

    knob->sliderAttachment = std::make_unique<SliderAttachment>(*(audioProcessor.apvts), paramID, knob->slider);
}