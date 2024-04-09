/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Editor::Editor (Processor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    gateKnob("GATE_KNOB_LABEL", "Gate Thresh", this),
    boostTopKnob("BOOST_TOP_KNOB_LABEL" , "Boost Top", this),
    boostTightKnob("BOOST_TIGHT_KNOB_LABEL", "Boost Tight", this),
    
    gainKnob("GAIN_KNOB_LABEL", "Pre Gain", this),

    inputFilterKnob("INPUT_KNOB_FILTER_LABEL", "Input Filter", this),
    
    bassEQKnob("BASS_EQ_KNOB_LABEL", "Bass", this),
    midEQKnob("MIDDLE_EQ_KNOB_LABEL", "Mid", this),
    trebbleEQKnob("TREBBLE_EQ_KNOB_LABEL", "Trebble", this),

    preampVolumeKnob("PREMP_VOLUME_KNOB_LABEL", "Post Gain", this),

    resonanceKnob("RESONANCE_KNOB_LABEL", "Resonance", this),
    presenceKnob("PRESENCE_KNOB_LABEL", "Presence", this),

    volumeKnob("MASTER_VOLUME_KNOB_LABEL", "Output Level", this),

    ampChannelBox("AMP_CHANNEL_BOX_LABEL", "Amp Channel", this),
    toneStackModelBox("TONE_MODEL_BOX_LABEL", "Tonestack Model", this)
{

    gateKnob.init(ParamIDs[GATE_THRESH].toString(), this);
    boostTopKnob.init(ParamIDs[BITE].toString(), this);
    boostTightKnob.init(ParamIDs[TIGHT].toString(), this);
    gainKnob.init(ParamIDs[PREAMP_GAIN].toString(), this);
    inputFilterKnob.init(ParamIDs[INPUT_FILTER].toString(), this);
    bassEQKnob.init(ParamIDs[TONESTACK_BASS].toString(), this);
    midEQKnob.init(ParamIDs[TONESTACK_MIDDLE].toString(), this);
    trebbleEQKnob.init(ParamIDs[TONESTACK_TREBBLE].toString(), this);
    preampVolumeKnob.init(ParamIDs[PREAMP_VOLUME].toString(), this);
    resonanceKnob.init(ParamIDs[RESONANCE].toString(), this);
    presenceKnob.init(ParamIDs[PRESENCE].toString(), this);
    volumeKnob.init(ParamIDs[MASTER_VOLUME].toString(), this);
    ampChannelBox.init(ParamIDs[CHANNEL].toString(), this);
    toneStackModelBox.init(ParamIDs[TONESTACK_MODEL].toString(), this);


    ampChannelBox.addItemList({"Channel 1", "Channel 2", "Channel 3", "Channel 4"}, 1);
    ampChannelBox.setSelectedId((int)*audioProcessor.apvts->getRawParameterValue(ParamIDs[CHANNEL]), 
                                 juce::NotificationType::dontSendNotification);

    toneStackModelBox.addItemList({"Savage", "JCM", "SLO", "Recto", "Orange"}, 1);
    toneStackModelBox.setSelectedId((int)*audioProcessor.apvts->getRawParameterValue(ParamIDs[TONESTACK_MODEL]), 
                                     juce::NotificationType::dontSendNotification);


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

    gateKnob.setBounds(computeXcoord(0), computeYcoord(0), knobSize, knobSize);
    gateKnob.label.setBounds(gateKnob.getX(), 
                              gateKnob.getY() - 20, 
                              gateKnob.getWidth(), 
                              20);

    boostTopKnob.setBounds(computeXcoord(0), computeYcoord(1), knobSize, knobSize);
    boostTopKnob.label.setBounds(boostTopKnob.getX(),
                                  boostTopKnob.getY() - 20,
                                  boostTopKnob.getWidth(), 
                                  20);

    boostTightKnob.setBounds(computeXcoord(0), computeYcoord(2), knobSize, knobSize);
    boostTightKnob.label.setBounds(boostTightKnob.getX(),
                                    boostTightKnob.getY() - 20,
                                    boostTightKnob.getWidth(),
                                    20);

    gainKnob.setBounds(computeXcoord(1), computeYcoord(1), knobSize, knobSize);
    gainKnob.label.setBounds(gainKnob.getX(), 
                              gainKnob.getY() - 20, 
                              gainKnob.getWidth(), 
                              20);
    
    inputFilterKnob.setBounds(computeXcoord(1), computeYcoord(2), knobSize, knobSize);
    inputFilterKnob.label.setBounds(inputFilterKnob.getX(), 
                                     inputFilterKnob.getY() - 20, 
                                     inputFilterKnob.getWidth(), 
                                     20);

    bassEQKnob.setBounds(computeXcoord(2), computeYcoord(1), knobSize, knobSize);
    bassEQKnob.label.setBounds(bassEQKnob.getX(), 
                                bassEQKnob.getY() - 20, 
                                bassEQKnob.getWidth(), 
                                20);

    midEQKnob.setBounds(computeXcoord(3), computeYcoord(1), knobSize, knobSize);
    midEQKnob.label.setBounds(midEQKnob.getX(), 
                               midEQKnob.getY() - 20, 
                               midEQKnob.getWidth(), 
                               20);
    
    trebbleEQKnob.setBounds(computeXcoord(4), computeYcoord(1), knobSize, knobSize);
    trebbleEQKnob.label.setBounds(trebbleEQKnob.getX(), 
                                   trebbleEQKnob.getY() - 20, 
                                   trebbleEQKnob.getWidth(), 
                                   20);

    resonanceKnob.setBounds(computeXcoord(5), computeYcoord(2), knobSize, knobSize);
    resonanceKnob.label.setBounds(resonanceKnob.getX(),
                                   resonanceKnob.getY() - 20,
                                   resonanceKnob.getWidth(), 
                                   20);

    presenceKnob.setBounds(computeXcoord(5), computeYcoord(1), knobSize, knobSize);
    presenceKnob.label.setBounds(presenceKnob.getX(),
                                   presenceKnob.getY() - 20,
                                   presenceKnob.getWidth(), 
                                   20);

    

    preampVolumeKnob.setBounds(computeXcoord(6), computeYcoord(1), knobSize, knobSize);
    preampVolumeKnob.label.setBounds(preampVolumeKnob.getX(), 
                                      preampVolumeKnob.getY() - 20, 
                                      preampVolumeKnob.getWidth(), 
                                      20);




    volumeKnob.setBounds(computeXcoord(6), computeYcoord(2), knobSize, knobSize);
    volumeKnob.label.setBounds(volumeKnob.getX(), 
                                volumeKnob.getY() - 20, 
                                volumeKnob.getWidth(), 
                                20);

    ampChannelBox.setBounds(computeXcoord(2), computeYcoord(2) + 30, 120, 30);
    ampChannelBox.label.setBounds(ampChannelBox.getX(),
                                   ampChannelBox.getY() - 20,
                                   ampChannelBox.getWidth(), 
                                   20);

    toneStackModelBox.setBounds(computeXcoord(3), computeYcoord(2) + 30, 120, 30);
    toneStackModelBox.label.setBounds(toneStackModelBox.getX(),
                                       toneStackModelBox.getY() - 20,
                                       toneStackModelBox.getWidth(), 
                                       20);



    irLoadButton.setBounds(computeXcoord(6), computeYcoord(0), 100, 50);
    irNameLabel.setBounds(irLoadButton.getX(), irLoadButton.getY() + irLoadButton.getHeight() + 5, 200, 20);

    irLoaderBypassToggle.setBounds(computeXcoord(5), computeYcoord(0), 100, 50);

    testOscToggle.setBounds(computeXcoord(0), computeYcoord(3), 100, 50);
    testOscNoiseToggle.setBounds(computeXcoord(1), computeYcoord(3), 100, 50);

    // tabs.setBounds(0, 0, 1000, 400);


}

Knob::Knob(juce::String labelID, juce::String name, Editor *editor) 
: label{labelID, name} 
{    
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 25);
    editor->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    editor->addAndMakeVisible(label);
}

void Knob::init(juce::String paramID, Editor *editor) {
    sliderAttachment = std::make_unique<SliderAttachment>(
        *(editor->audioProcessor.apvts), paramID, *this
    );
}


ComboBox::ComboBox(juce::String labelID, juce::String name, Editor *editor) 
         : label {labelID, name}
{
    setEditableText(false);
    setJustificationType(juce::Justification::centred);
    editor->addAndMakeVisible(*this);
    
    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    
    editor->addAndMakeVisible(label);
    
}

void ComboBox::init(juce::String paramID, Editor *editor) {
    boxAttachment = std::make_unique<ComboBoxAttachment>(
        *(editor->audioProcessor.apvts), paramID, *this
    );
}
