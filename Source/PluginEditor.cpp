/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Editor::Editor (Processor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    gateKnob = std::make_unique<Knob>("GATE_KNOB_LABEL", "Gate Thresh", ParamIDs[GATE_THRESH], this);
    boostTopKnob = std::make_unique<Knob>("BOOST_TOP_KNOB_LABEL" , "Boost Top", ParamIDs[BITE], this);
    boostTightKnob = std::make_unique<Knob>("BOOST_TIGHT_KNOB_LABEL", "Boost Tight", ParamIDs[TIGHT], this);
    
    gainKnob = std::make_unique<Knob>("GAIN_KNOB_LABEL", "Pre Gain", ParamIDs[PREAMP_GAIN], this);

    inputFilterKnob = std::make_unique<Knob>("INPUT_KNOB_FILTER_LABEL", "Input Filter", ParamIDs[INPUT_FILTER], this);
    
    bassEQKnob = std::make_unique<Knob>("BASS_EQ_KNOB_LABEL", "Bass", ParamIDs[TONESTACK_BASS], this);
    midEQKnob = std::make_unique<Knob>("MIDDLE_EQ_KNOB_LABEL", "Mid", ParamIDs[TONESTACK_MIDDLE], this);
    trebbleEQKnob = std::make_unique<Knob>("TREBBLE_EQ_KNOB_LABEL", "Trebble", ParamIDs[TONESTACK_TREBBLE], this);

    preampVolumeKnob = std::make_unique<Knob>("PREMP_VOLUME_KNOB_LABEL", "Post Gain", ParamIDs[PREAMP_VOLUME], this);

    resonanceKnob = std::make_unique<Knob>("RESONANCE_KNOB_LABEL", "Resonance", ParamIDs[RESONANCE], this);
    presenceKnob = std::make_unique<Knob>("PRESENCE_KNOB_LABEL", "Presence", ParamIDs[PRESENCE], this);

    volumeKnob = std::make_unique<Knob>("MASTER_VOLUME_KNOB_LABEL", "Output Level", ParamIDs[MASTER_VOLUME], this);

    ampChannelBox = std::make_unique<ComboBox>("AMP_CHANNEL_BOX_LABEL", "Amp Channel", ParamIDs[CHANNEL], this);
    toneStackModelBox = std::make_unique<ComboBox>("TONE_MODEL_BOX_LABEL", "Tonestack Model", ParamIDs[TONESTACK_MODEL], this);


    ampChannelBox->box.addItemList({"Channel 1", "Channel 2", "Channel 3", "Channel 4"}, 1);
    ampChannelBox->box.setSelectedId(2, juce::NotificationType::dontSendNotification);

    toneStackModelBox->box.addItemList({"Savage", "JCM", "SLO", "Recto", "Orange"}, 1);
    toneStackModelBox->box.setSelectedId(1, juce::NotificationType::dontSendNotification);


    irLoadButton.onClick = [&]() { audioProcessor.irLoader->loadIR(false, &irNameLabel); };
    addAndMakeVisible(irLoadButton);

    irNameLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    irNameLabel.setJustificationType(juce::Justification::left);
    irNameLabel.setFont(15.0f);
    addAndMakeVisible(irNameLabel);

    irLoaderBypassToggle.onClick = [&]() {
        audioProcessor.irLoader->bypass = irLoaderBypassToggle.getToggleState();
    };

    addAndMakeVisible(irLoaderBypassToggle);

    testOscToggle.onClick = [&]() { 
        audioProcessor.doTestOsc = testOscToggle.getToggleState(); 
    }; 

    testOscNoiseToggle.onClick = [&]() { 
        audioProcessor.testOsc.doNoise = testOscNoiseToggle.getToggleState(); 
    };


    addAndMakeVisible(testOscToggle);
    addAndMakeVisible(testOscNoiseToggle);

    tabs.addTab("Tab 1", juce::Colours::red, &testOscToggle, false, 0);
    tabs.addTab("Tab 2", juce::Colours::blue, &testOscNoiseToggle, false, 1);
    // addAndMakeVisible(tabs);


    setSize(1200, 600);
    setResizable(true, true);
}

Editor::~Editor()
{
}

//==============================================================================
void Editor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
}

void Editor::resized() {
    
    int knobSize = 100;
    int horizontalMargin = 25;
    int verticalMargin = 50;

    int HEIGHT = getHeight() - 2*verticalMargin;
    int WIDTH = getWidth() - 2*horizontalMargin;

    int nRows = 4;
    int nCols = 8;

    auto computeXcoord = [&](int col) { return horizontalMargin + int(WIDTH/nCols) * col; };
    auto computeYcoord = [&](int row) { return verticalMargin + int(HEIGHT/nRows) * row; };

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

    resonanceKnob->slider.setBounds(computeXcoord(5), computeYcoord(2), knobSize, knobSize);
    resonanceKnob->label.setBounds(resonanceKnob->slider.getX(),
                                   resonanceKnob->slider.getY() - 20,
                                   resonanceKnob->slider.getWidth(), 
                                   20);

    presenceKnob->slider.setBounds(computeXcoord(5), computeYcoord(1), knobSize, knobSize);
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

    ampChannelBox->box.setBounds(computeXcoord(2), computeYcoord(2) + 30, 120, 30);
    ampChannelBox->label.setBounds(ampChannelBox->box.getX(),
                                   ampChannelBox->box.getY() - 20,
                                   ampChannelBox->box.getWidth(), 
                                   20);

    toneStackModelBox->box.setBounds(computeXcoord(3), computeYcoord(2) + 30, 120, 30);
    toneStackModelBox->label.setBounds(toneStackModelBox->box.getX(),
                                       toneStackModelBox->box.getY() - 20,
                                       toneStackModelBox->box.getWidth(), 
                                       20);



    irLoadButton.setBounds(computeXcoord(6), computeYcoord(0), 100, 50);
    irNameLabel.setBounds(irLoadButton.getX(), irLoadButton.getY() + irLoadButton.getHeight() + 5, 200, 20);

    irLoaderBypassToggle.setBounds(computeXcoord(5), computeYcoord(0), 100, 50);

    testOscToggle.setBounds(computeXcoord(0), computeYcoord(3), 100, 50);
    testOscNoiseToggle.setBounds(computeXcoord(1), computeYcoord(3), 100, 50);

    // tabs.setBounds(0, 0, 1000, 400);


}
