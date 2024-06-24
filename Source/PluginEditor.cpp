/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


static int computeXcoord(int col, int height) { 
    return horizontalMargin + int((height - 2*verticalMargin)/nCols) * col; 
}

static int computeYcoord(int row, int width) { 
    return verticalMargin + int((width - 2*horizontalMargin)/nRows) * row; 
}


GateBoostPage::GateBoostPage(Apvts &apvts) :
    gateKnob("GATE_KNOB_LABEL", "Gate Thresh", this),
    gateAttackKnob("GATE_Attack_LABEL", "Gate Attack", this),
    gateReleaseKnob("GATE_RELEASE_LABEL", "Gate Release", this), 
    gateReturnKnob("GATE_RETURN_LABEL", "Gate Hysteresis", this),

    boostAttackKnob("BOOST_ATTACK_KNOB_LABEL" , "Boost Top", this),
    boostFreqKnob("BOOST_FREQ_KNOB_LABEL", "Boost Freq", this),
    boostTightKnob("BOOST_TIGHT_KNOB_LABEL", "Boost Tight", this)
{
    gateKnob.init(ParamIDs[GATE_THRESH].toString(), apvts);
    gateKnob.setTextValueSuffix(" dB");

    gateAttackKnob.init(ParamIDs[GATE_ATTACK].toString(), apvts);
    gateAttackKnob.setTextValueSuffix(" ms");
    
    gateReleaseKnob.init(ParamIDs[GATE_RELEASE].toString(), apvts);
    gateReleaseKnob.setTextValueSuffix(" ms");
    
    gateReturnKnob.init(ParamIDs[GATE_RETURN].toString(), apvts);
    gateReleaseKnob.setTextValueSuffix(" ms");


    boostAttackKnob.init(ParamIDs[BITE].toString(), apvts);
    boostAttackKnob.setTextValueSuffix(" dB");
    
    boostFreqKnob.init(ParamIDs[BITE_FREQ].toString(), apvts);
    boostFreqKnob.setTextValueSuffix(" Hz");
    
    boostTightKnob.init(ParamIDs[TIGHT].toString(), apvts);
    boostTightKnob.setTextValueSuffix(" Hz");

}

void GateBoostPage::resized() {

    int width = getWidth();
    int height = getHeight();
    

    gateKnob.setBounds(computeXcoord(0, width), computeYcoord(0, height), knobSize, knobSize);
    gateKnob.label.setBounds(gateKnob.getX(), 
                              gateKnob.getY() - 15, 
                              gateKnob.getWidth(), 
                              20);

    gateAttackKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    gateAttackKnob.label.setBounds(gateAttackKnob.getX(),
                                   gateAttackKnob.getY() - 15, 
                                   gateAttackKnob.getWidth(), 
                                   20);
                                    
    gateReleaseKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    gateReleaseKnob.label.setBounds(gateReleaseKnob.getX(),
                                    gateReleaseKnob.getY() - 15, 
                                    gateReleaseKnob.getWidth(), 
                                    20);
                                    
    gateReturnKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    gateReturnKnob.label.setBounds(gateReturnKnob.getX(),
                                   gateReturnKnob.getY() - 15, 
                                   gateReturnKnob.getWidth(), 
                                   20);
                                    


    boostAttackKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), knobSize, knobSize);
    boostAttackKnob.label.setBounds(boostAttackKnob.getX(),
                                  boostAttackKnob.getY() - 15,
                                  boostAttackKnob.getWidth(), 
                                  20);
    
    boostFreqKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), knobSize, knobSize);
    boostFreqKnob.label.setBounds(boostFreqKnob.getX(),
                                  boostFreqKnob.getY() - 15,
                                  boostFreqKnob.getWidth(), 
                                  20);

    boostTightKnob.setBounds(computeXcoord(2, width), computeYcoord(1, height), knobSize, knobSize);
    boostTightKnob.label.setBounds(boostTightKnob.getX(),
                                    boostTightKnob.getY() - 15,
                                    boostTightKnob.getWidth(),
                                    20);
}


AmplifierPage::AmplifierPage(Apvts &apvts) :
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
    toneStackModelBox("TONE_MODEL_BOX_LABEL", "Tonestack Model", this),
    channelConfigBox("CHANNEL_CONFIG_BOX_LABEL", "Channel config", this)
{
    gainKnob.init(ParamIDs[PREAMP_GAIN].toString(), apvts);
    gainKnob.setNumDecimalPlacesToDisplay(2);
    inputFilterKnob.init(ParamIDs[INPUT_FILTER].toString(), apvts);
    inputFilterKnob.setTextValueSuffix(" Hz");
    
    bassEQKnob.init(ParamIDs[TONESTACK_BASS].toString(), apvts);
    
    midEQKnob.init(ParamIDs[TONESTACK_MIDDLE].toString(), apvts);
    
    trebbleEQKnob.init(ParamIDs[TONESTACK_TREBBLE].toString(), apvts);
    
    preampVolumeKnob.init(ParamIDs[PREAMP_VOLUME].toString(), apvts);
    preampVolumeKnob.setTextValueSuffix(" dB");
 
    resonanceKnob.init(ParamIDs[RESONANCE].toString(), apvts);
    presenceKnob.init(ParamIDs[PRESENCE].toString(), apvts);
    volumeKnob.init(ParamIDs[MASTER_VOLUME].toString(), apvts);
    volumeKnob.setTextValueSuffix(" dB");
 
    ampChannelBox.init(ParamIDs[CHANNEL].toString(), apvts);
    toneStackModelBox.init(ParamIDs[TONESTACK_MODEL].toString(), apvts);
    channelConfigBox.init(ParamIDs[CHANNEL_CONFIG].toString(), apvts);

    ampChannelBox.addItemList({"Channel 1", "Channel 2", "Channel 3", "Channel 4"}, 1);
    ampChannelBox.setSelectedId((int)*apvts.getRawParameterValue(ParamIDs[CHANNEL]), 
                                 juce::NotificationType::dontSendNotification);

    toneStackModelBox.addItemList({"Savage", "JCM", "SLO", "Recto", "Orange"}, 1);
    toneStackModelBox.setSelectedId((int)*apvts.getRawParameterValue(ParamIDs[TONESTACK_MODEL]) + 1, 
                                     juce::NotificationType::dontSendNotification);

    channelConfigBox.addItemList({"Mono","Fake Stereo", "Stereo"}, 1);
    channelConfigBox.setSelectedId((int)*apvts.getRawParameterValue(ParamIDs[CHANNEL_CONFIG]) + 1,
                                    juce::NotificationType::dontSendNotification);

}


void AmplifierPage::resized() {

    int width = getWidth();
    int height = getHeight();
    
    gainKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    gainKnob.label.setBounds(gainKnob.getX(), 
                              gainKnob.getY() - 15, 
                              gainKnob.getWidth(), 
                              20);
    
    inputFilterKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    inputFilterKnob.label.setBounds(inputFilterKnob.getX(), 
                                     inputFilterKnob.getY() - 15, 
                                     inputFilterKnob.getWidth(), 
                                     20);

    bassEQKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), knobSize, knobSize);
    bassEQKnob.label.setBounds(bassEQKnob.getX(), 
                                bassEQKnob.getY() - 15, 
                                bassEQKnob.getWidth(), 
                                20);

    midEQKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), knobSize, knobSize);
    midEQKnob.label.setBounds(midEQKnob.getX(), 
                               midEQKnob.getY() - 15, 
                               midEQKnob.getWidth(), 
                               20);
    
    trebbleEQKnob.setBounds(computeXcoord(4, width), computeYcoord(0, height), knobSize, knobSize);
    trebbleEQKnob.label.setBounds(trebbleEQKnob.getX(), 
                                   trebbleEQKnob.getY() - 15, 
                                   trebbleEQKnob.getWidth(), 
                                   20);

    resonanceKnob.setBounds(computeXcoord(5, width), computeYcoord(1, height), knobSize, knobSize);
    resonanceKnob.label.setBounds(resonanceKnob.getX(),
                                   resonanceKnob.getY() - 15,
                                   resonanceKnob.getWidth(), 
                                   20);

    presenceKnob.setBounds(computeXcoord(5, width), computeYcoord(0, height), knobSize, knobSize);
    presenceKnob.label.setBounds(presenceKnob.getX(),
                                   presenceKnob.getY() - 15,
                                   presenceKnob.getWidth(), 
                                   20);

    

    preampVolumeKnob.setBounds(computeXcoord(6, width), computeYcoord(0, height), knobSize, knobSize);
    preampVolumeKnob.label.setBounds(preampVolumeKnob.getX(), 
                                      preampVolumeKnob.getY() - 15, 
                                      preampVolumeKnob.getWidth(), 
                                      20);




    volumeKnob.setBounds(computeXcoord(6, width), computeYcoord(1, height), knobSize, knobSize);
    volumeKnob.label.setBounds(volumeKnob.getX(), 
                                volumeKnob.getY() - 15, 
                                volumeKnob.getWidth(), 
                                20);

    ampChannelBox.setBounds(computeXcoord(2, width), computeYcoord(1, height) + 30, 120, 30);
    ampChannelBox.label.setBounds(ampChannelBox.getX(),
                                   ampChannelBox.getY() - 20,
                                   ampChannelBox.getWidth(), 
                                   20);

    toneStackModelBox.setBounds(computeXcoord(3, width), computeYcoord(1, height) + 30, 120, 30);
    toneStackModelBox.label.setBounds(toneStackModelBox.getX(),
                                       toneStackModelBox.getY() - 20,
                                       toneStackModelBox.getWidth(), 
                                       20);

    channelConfigBox.setBounds(computeXcoord(0, width), computeYcoord(0, height) + 30, 120, 30);
    channelConfigBox.label.setBounds(channelConfigBox.getX(),
                                      channelConfigBox.getY() - 20,
                                      channelConfigBox.getWidth(),
                                      20);

}


IRLoaderPage::IRLoaderPage(Processor &audioProcessor) {

    irLoadButton.onClick = [&]() { 
        auto chooser = std::make_unique<juce::FileChooser>("Choose a .wav File to open", juce::File(), "*.wav");

        bool fileChoosed = chooser->browseForFileToOpen();
        if (!fileChoosed) { 
            return; 
        }

        juce::File returnedFile = chooser->getResult();
        
        if (returnedFile.getFileExtension() != ".wav") {
            return;
        }

        audioProcessor.irLoader->irFile = returnedFile;
        IRLoaderError error = audioProcessor.irLoader->loadIR(false); 
        
        if (error == IRLoaderError::Error) { return; }
    
        irNameLabel.setText(returnedFile.getFileNameWithoutExtension(),
                            juce::NotificationType::dontSendNotification);
                            
        audioProcessor.valueTree.setProperty(irPath1, returnedFile.getFullPathName(), nullptr);
    };
    
    addAndMakeVisible(irLoadButton);

    irNameLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    irNameLabel.setJustificationType(juce::Justification::left);
    irNameLabel.setFont(15.0f);
    
    juce::String irNameText = audioProcessor.irLoader->irFile.getFileNameWithoutExtension();
        
        
    if (irNameText == "") {
        irNameLabel.setText(defaultIRText,
                            juce::NotificationType::dontSendNotification);        
    } else { 
        irNameLabel.setText(audioProcessor.irLoader->irFile.getFileNameWithoutExtension(), 
                            juce::NotificationType::dontSendNotification);
    }
    
    addAndMakeVisible(irNameLabel);

    irLoaderDefaultIRButton.onClick = [&]() {
        IRLoaderError error = audioProcessor.irLoader->loadIR(true);

        if (error == IRLoaderError::Error) { return; }
    
        irNameLabel.setText(defaultIRText,
                            juce::NotificationType::dontSendNotification);
                            
        audioProcessor.valueTree.setProperty(irPath1, "", nullptr);
    };
    
    addAndMakeVisible(irLoaderDefaultIRButton);


    irLoaderBypassToggle.setToggleState(audioProcessor.irLoader->bypass, 
                                        juce::NotificationType::dontSendNotification);
    irLoaderBypassToggle.onClick = [&]() {
        audioProcessor.irLoader->bypass = irLoaderBypassToggle.getToggleState();
    };

    addAndMakeVisible(irLoaderBypassToggle);

}

void IRLoaderPage::resized() {

    int width = getWidth();
    int height = getHeight();

    irLoadButton.setBounds(computeXcoord(0, width), computeYcoord(0, height), 100, 50);

    irLoaderBypassToggle.setBounds(computeXcoord(1, width), computeYcoord(0, height) - 20, 120, 30);
    irLoaderDefaultIRButton.setBounds(irLoaderBypassToggle.getX(), irLoaderBypassToggle.getY() + 40,
                                      120, 30);
                                      
    irNameLabel.setBounds(irLoadButton.getX(), 
                          irLoadButton.getY() + irLoadButton.getHeight() + 5, 
                          300, 20);
}


//==============================================================================
Editor::Editor (Processor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    gateBoostPage(p.apvts),
    ampPage(p.apvts),
    irLoaderPage(p)
{


    juce::Colour colour = findColour (juce::ResizableWindow::backgroundColourId);
    tabs.addTab("Gate Boost", colour, &gateBoostPage,  true);
    tabs.addTab("Amp",        colour, &ampPage,        true);
    tabs.addTab("GainStages", colour, &gainStagesPage, true);
    tabs.addTab("IRLoader",   colour, &irLoaderPage,   true);
    
    addAndMakeVisible(tabs);


    setSize(1100, 500);
    setResizable(true, true);
}

//==============================================================================
void Editor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
}

void Editor::resized() {
    
    tabs.setBounds(getLocalBounds());

    gateBoostPage.resized();
    ampPage.resized();
}


Knob::Knob(juce::String labelID, juce::String name, juce::Component *comp) 
: label{labelID, name} 
{    
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    comp->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);
}

void Knob::init(juce::String paramID, Apvts &apvts) {
    sliderAttachment = std::make_unique<SliderAttachment>(
        apvts, paramID, *this
    );
}


ComboBox::ComboBox(juce::String labelID, juce::String name, juce::Component *comp) 
         : label {labelID, name}
{
    setEditableText(false);
    setJustificationType(juce::Justification::centred);
    comp->addAndMakeVisible(*this);
    
    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    
    comp->addAndMakeVisible(label);
    
}

void ComboBox::init(juce::String paramID, Apvts &apvts) {
    boxAttachment = std::make_unique<ComboBoxAttachment>(
        apvts, paramID, *this
    );
}
