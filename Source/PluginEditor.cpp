/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


static int computeXcoord(float col, int width) {    
    int usableWidth = width - 2*horizontalMargin - knobSize;
    return horizontalMargin + int(usableWidth/nCols * col); 
}

static int computeYcoord(float row, int height) { 
    return verticalMargin + int((height - 2*verticalMargin)/nRows * row); 
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
    gateReturnKnob.setTextValueSuffix(" dB");


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

    irLoadButton.onClick = [&audioProcessor, this]() { 
        auto chooser = std::make_unique<juce::FileChooser>("Choose a .wav File to open", 
                                                           juce::File(), "*.wav");

        bool fileChoosed = chooser->browseForFileToOpen();
        if (!fileChoosed) { 
            return; 
        }

        juce::File returnedFile = chooser->getResult();
        
        if (returnedFile.getFileExtension() != ".wav") {
            return;
        }

        audioProcessor.irLoader.irFile = returnedFile;
        IRLoaderError error = audioProcessor.irLoader.loadIR(false); 
        
        if (error == IRLoaderError::Error) { return; }
    
        irNameLabel.setText(returnedFile.getFileNameWithoutExtension(),
                            juce::NotificationType::dontSendNotification);
                            
        audioProcessor.valueTree.setProperty(irPath1, returnedFile.getFullPathName(), nullptr);
    
        // get the list of files in the same directory
        juce::File parentFolder = returnedFile.getParentDirectory();
        audioProcessor.irLoader.directoryWavFiles = parentFolder.findChildFiles(
            juce::File::TypesOfFileToFind::findFiles,
            false, 
            "*.wav",
            juce::File::FollowSymlinks::no
        );
        
        audioProcessor.irLoader.indexOfCurrentFile = audioProcessor.irLoader.directoryWavFiles.indexOf(returnedFile);
    };
    
    addAndMakeVisible(irLoadButton);
    
    nextIRButton.onClick = [&audioProcessor, this]() {
        
        // if the defualt ir is loader, ignore the button
        if (audioProcessor.irLoader.defaultIR) { return; }
    
        audioProcessor.irLoader.indexOfCurrentFile = (audioProcessor.irLoader.indexOfCurrentFile + 1) % audioProcessor.irLoader.directoryWavFiles.size();
        
        juce::File fileToLoad = audioProcessor.irLoader.directoryWavFiles[audioProcessor.irLoader.indexOfCurrentFile];
        audioProcessor.irLoader.irFile = fileToLoad;
        IRLoaderError error = audioProcessor.irLoader.loadIR(false);

        if (error == IRLoaderError::Error) { return; }
    
        irNameLabel.setText(fileToLoad.getFileNameWithoutExtension(),
                            juce::NotificationType::dontSendNotification);
                            
        audioProcessor.valueTree.setProperty(irPath1, fileToLoad.getFullPathName(), nullptr);        
    
    };
    
    prevIRButton.onClick = [&audioProcessor, this]() {
        
        if (audioProcessor.irLoader.defaultIR) { return; }
        
        audioProcessor.irLoader.indexOfCurrentFile--; 
        if (audioProcessor.irLoader.indexOfCurrentFile < 0) {
            audioProcessor.irLoader.indexOfCurrentFile +=  audioProcessor.irLoader.directoryWavFiles.size();
        }
        
        juce::File fileToLoad = audioProcessor.irLoader.directoryWavFiles[audioProcessor.irLoader.indexOfCurrentFile];
        audioProcessor.irLoader.irFile = fileToLoad;
        IRLoaderError error = audioProcessor.irLoader.loadIR(false);

        if (error == IRLoaderError::Error) { return; }
    
        irNameLabel.setText(fileToLoad.getFileNameWithoutExtension(),
                            juce::NotificationType::dontSendNotification);
                            
        audioProcessor.valueTree.setProperty(irPath1, fileToLoad.getFullPathName(), nullptr);        
    };
    
    addAndMakeVisible(nextIRButton);
    addAndMakeVisible(prevIRButton);
    
    irNameLabel.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    irNameLabel.setJustificationType(juce::Justification::left);
    irNameLabel.setFont(15.0f);
    
    juce::String irNameText = audioProcessor.irLoader.irFile.getFileNameWithoutExtension();
        
        
    if (audioProcessor.irLoader.defaultIR) {
        irNameLabel.setText(defaultIRText,
                            juce::NotificationType::dontSendNotification);        
    } else { 
        irNameLabel.setText(irNameText,
                            juce::NotificationType::dontSendNotification);
    }
    
    addAndMakeVisible(irNameLabel);

    irLoaderDefaultIRButton.onClick = [&audioProcessor, this]() {
        IRLoaderError error = audioProcessor.irLoader.loadIR(true);

        if (error == IRLoaderError::Error) { return; }
    
        irNameLabel.setText(defaultIRText,
                            juce::NotificationType::dontSendNotification);
                            
        audioProcessor.valueTree.setProperty(irPath1, "", nullptr);
    };
    
    addAndMakeVisible(irLoaderDefaultIRButton);


    irLoaderBypassToggle.setToggleState(audioProcessor.irLoader.bypass, 
                                        juce::NotificationType::dontSendNotification);
    irLoaderBypassToggle.onClick = [&audioProcessor, this]() {
        audioProcessor.irLoader.bypass = irLoaderBypassToggle.getToggleState();
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
                          400, 50);

    nextIRButton.setBounds(computeXcoord(0, width), computeYcoord(1, height), 120, 30);
    prevIRButton.setBounds(nextIRButton.getX(), nextIRButton.getY() + 50, 120, 30);

}


//==============================================================================
Editor::Editor (Processor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    gateBoostPage(p.apvts),
    ampPage(p.apvts),
    irLoaderPage(p), 
    volumePanel(p)
{


    juce::Colour colour = findColour (juce::ResizableWindow::backgroundColourId);
    tabs.addTab("Gate Boost", colour, &gateBoostPage,  true);
    tabs.addTab("Amp",        colour, &ampPage,        true);
    tabs.addTab("GainStages", colour, &gainStagesPage, true);
    tabs.addTab("IRLoader",   colour, &irLoaderPage,   true);
    
    addAndMakeVisible(tabs);
    addAndMakeVisible(volumePanel);

    setSize(1100, 500);
    setResizable(true, true);
}

//==============================================================================
void Editor::paint (juce::Graphics& g)
{
	g.fillAll(juce::Colours::darkgrey);
    volumePanel.paint(g);
}

void Editor::resized() {
    
    juce::Rectangle<int> bounds = getLocalBounds();
    
    volumePanel.setBounds(bounds.removeFromRight(100));

    gateBoostPage.setBounds(bounds);
    ampPage.setBounds(bounds);
    irLoaderPage.setBounds(bounds);
    
    tabs.setBounds(bounds);
    

    gateBoostPage.resized();
    ampPage.resized();
    irLoaderPage.resized();
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


Slider::Slider(juce::String labelID, juce::String name, juce::Component *comp) 
: label{labelID, name} 
{    
    setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    comp->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);
}

void Slider::init(juce::String paramID, Apvts &apvts) {
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
