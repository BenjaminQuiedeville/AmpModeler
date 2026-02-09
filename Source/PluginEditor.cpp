/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginEditor.h"


static inline int computeXcoord(float col, int width) {
    int usableWidth = width - 2*horizontalMargin - knobSize;
    return horizontalMargin + int(usableWidth/nCols * (int)col);
}

static inline int computeYcoord(float row, int height) {
    return verticalMargin + int((height - 2*verticalMargin)/nRows * (int)row);
}


GateBoostPage::GateBoostPage(Processor &p) :
    gateKnob("GATE_KNOB_LABEL",                "Gate Thresh",     this, paramInfos[GATE_THRESH].id.toString(),     p.apvts, " dB"),
    // gateAttackKnob("GATE_Attack_LABEL",        "Gate Attack",     this, paramInfos[GATE_ATTACK].id.toString(),     p.apvts, " ms"),
    // gateReleaseKnob("GATE_RELEASE_LABEL",      "Gate Release",    this, paramInfos[GATE_RELEASE].id.toString(),    p.apvts, " ms"),
    // gateReturnKnob("GATE_RETURN_LABEL",        "Gate Hysteresis", this, paramInfos[GATE_RETURN].id.toString(),     p.apvts, " dB"),

    boostAttackKnob("BOOST_ATTACK_KNOB_LABEL", "Boost Top",       this, paramInfos[SCREAMER_AMOUNT].id.toString(), p.apvts, " dB"),
    boostFreqKnob("BOOST_FREQ_KNOB_LABEL",     "Boost Freq",      this, paramInfos[SCREAMER_FREQ].id.toString(),   p.apvts, " Hz"),
    boostTightKnob("BOOST_TIGHT_KNOB_LABEL",   "Boost Tight",     this, paramInfos[TIGHT].id.toString(),           p.apvts, " Hz"),
    inputGainKnob("INPUT_GAIN_KNOB_LABEL",     "Input Gain",      this, paramInfos[INPUT_GAIN].id.toString(),      p.apvts, " dB")

{
    gateToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[GATE_ACTIVE].id.toString(), gateToggle
    );
    preampToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[PREAMP_ACTIVE].id.toString(), preampToggle
    );
    tonestackToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[TONESTACK_ACTIVE].id.toString(), tonestackToggle
    );
    EQToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[EQ_ACTIVE].id.toString(), EQToggle
    );

    addAndMakeVisible(gateToggle);
    addAndMakeVisible(preampToggle);
    addAndMakeVisible(tonestackToggle);
    addAndMakeVisible(EQToggle);
}


void GateBoostPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();


    inputGainKnob.setBounds(computeXcoord(0, width), computeYcoord(0, height), knobSize, knobSize);
    inputGainKnob.label.setBounds(inputGainKnob.getX(), inputGainKnob.getY() - 15, knobSize, 20);

    gateKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    gateKnob.label.setBounds(gateKnob.getX(), gateKnob.getY() - 15, knobSize, 20);


    // gateAttackKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    // gateAttackKnob.label.setBounds(gateAttackKnob.getX(), gateAttackKnob.getY() - 15, knobSize, 20);

    // gateReleaseKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReleaseKnob.label.setBounds(gateReleaseKnob.getX(), gateReleaseKnob.getY() - 15, knobSize, 20);

    // gateReturnKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReturnKnob.label.setBounds(gateReturnKnob.getX(), gateReturnKnob.getY() - 15, knobSize, 20);


    boostAttackKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), knobSize, knobSize);
    boostAttackKnob.label.setBounds(boostAttackKnob.getX(), boostAttackKnob.getY() - 15, knobSize, 20);

    boostFreqKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), knobSize, knobSize);
    boostFreqKnob.label.setBounds(boostFreqKnob.getX(), boostFreqKnob.getY() - 15, knobSize, 20);

    boostTightKnob.setBounds(computeXcoord(2, width), computeYcoord(1, height), knobSize, knobSize);
    boostTightKnob.label.setBounds(boostTightKnob.getX(), boostTightKnob.getY() - 15, knobSize, 20);

    gateToggle.setBounds(computeXcoord(4, width), computeYcoord(0, height), 200, 30);
    preampToggle.setBounds(gateToggle.getBounds() + juce::Point(0, gateToggle.getHeight() + 20));
    tonestackToggle.setBounds(preampToggle.getBounds() + juce::Point(0, preampToggle.getHeight() + 20));
    EQToggle.setBounds(tonestackToggle.getBounds() + juce::Point(0, tonestackToggle.getHeight() + 20));
}


AmplifierPage::AmplifierPage(Processor &p) :
    gainKnob("GAIN_KNOB_LABEL",                  "Pre Gain",        this, paramInfos[PREAMP_GAIN].id.toString(),       p.apvts, ""),

    inputFilterKnob("INPUT_KNOB_FILTER_LABEL",   "Input Filter",    this, paramInfos[INPUT_FILTER].id.toString(),      p.apvts, " Hz"),

    bassEQKnob("BASS_EQ_KNOB_LABEL",             "Bass",            this, paramInfos[TONESTACK_BASS].id.toString(),    p.apvts, ""),
    midEQKnob("MIDDLE_EQ_KNOB_LABEL",            "Mid",             this, paramInfos[TONESTACK_MIDDLE].id.toString(),  p.apvts, ""),
    trebbleEQKnob("TREBBLE_EQ_KNOB_LABEL",       "Trebble",         this, paramInfos[TONESTACK_TREBBLE].id.toString(), p.apvts, ""),

    preampVolumeKnob("PREMP_VOLUME_KNOB_LABEL",  "Post Gain",       this, paramInfos[PREAMP_VOLUME].id.toString(),     p.apvts, " dB"),

    resonanceKnob("RESONANCE_KNOB_LABEL",        "Resonance",       this, paramInfos[RESONANCE].id.toString(),         p.apvts, ""),
    presenceKnob("PRESENCE_KNOB_LABEL",          "Presence",        this, paramInfos[PRESENCE].id.toString(),          p.apvts, ""),

    ampChannelBox("AMP_CHANNEL_BOX_LABEL",       "Amp Channel",     this, paramInfos[CHANNEL].id.toString(),           p.apvts),
    toneStackModelBox("TONE_MODEL_BOX_LABEL",    "Tonestack Model", this, paramInfos[TONESTACK_MODEL].id.toString(),   p.apvts),
    channelConfigBox("CHANNEL_CONFIG_BOX_LABEL", "Channel config",  this, paramInfos[CHANNEL_CONFIG].id.toString(),    p.apvts)
{
    gainKnob.setNumDecimalPlacesToDisplay(2);

    ampChannelBox.addItemList({"Channel 1", "Channel 2", "Channel 3", "Channel 4"}, 1);
    ampChannelBox.setSelectedId((int)*p.apvts.getRawParameterValue(paramInfos[CHANNEL].id),
                                 juce::NotificationType::dontSendNotification);

    toneStackModelBox.addItemList({"Normal", "Scooped", "Mids!"}, 1);
    toneStackModelBox.setSelectedId((int)*p.apvts.getRawParameterValue(paramInfos[TONESTACK_MODEL].id) + 1,
                                     juce::NotificationType::dontSendNotification);

    channelConfigBox.addItemList({"Mono","Fake Stereo", "Stereo"}, 1);
    channelConfigBox.setSelectedId((int)*p.apvts.getRawParameterValue(paramInfos[CHANNEL_CONFIG].id) + 1,
                                    juce::NotificationType::dontSendNotification);
    
    // channelConfigBox.valueChanged = [&, this](juce::Value &value) {
        
    //     value;
    //     // changer le style des potard dans le gainstages 
    // };

    brightToggleAttachment = std::make_unique<ButtonAttachment>(
        p.apvts, paramInfos[BRIGHT_CAP].id.toString(), brightToggle
    );
    
    addAndMakeVisible(brightToggle);
}


void AmplifierPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();

    gainKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    gainKnob.label.setBounds(gainKnob.getX(), gainKnob.getY() - 15, knobSize, 20);

    inputFilterKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    inputFilterKnob.label.setBounds(inputFilterKnob.getX(), inputFilterKnob.getY() - 15, knobSize, 20);

    bassEQKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), knobSize, knobSize);
    bassEQKnob.label.setBounds(bassEQKnob.getX(), bassEQKnob.getY() - 15, knobSize, 20);

    midEQKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), knobSize, knobSize);
    midEQKnob.label.setBounds(midEQKnob.getX(), midEQKnob.getY() - 15, knobSize, 20);

    trebbleEQKnob.setBounds(computeXcoord(4, width), computeYcoord(0, height), knobSize, knobSize);
    trebbleEQKnob.label.setBounds(trebbleEQKnob.getX(), trebbleEQKnob.getY() - 15, knobSize, 20);

    resonanceKnob.setBounds(computeXcoord(5, width), computeYcoord(1, height), knobSize, knobSize);
    resonanceKnob.label.setBounds(resonanceKnob.getX(), resonanceKnob.getY() - 15, knobSize, 20);

    presenceKnob.setBounds(computeXcoord(5, width), computeYcoord(0, height), knobSize, knobSize);
    presenceKnob.label.setBounds(presenceKnob.getX(), presenceKnob.getY() - 15, knobSize, 20);


    preampVolumeKnob.setBounds(computeXcoord(6, width), computeYcoord(0, height), knobSize, knobSize);
    preampVolumeKnob.label.setBounds(preampVolumeKnob.getX(), preampVolumeKnob.getY() - 15, knobSize, 20);


    ampChannelBox.setBounds(computeXcoord(2, width), computeYcoord(1, height) + 30, 120, 30);
    ampChannelBox.label.setBounds(ampChannelBox.getX(), ampChannelBox.getY() - 20, knobSize, 20);

    toneStackModelBox.setBounds(computeXcoord(3, width), computeYcoord(1, height) + 30, 120, 30);
    toneStackModelBox.label.setBounds(toneStackModelBox.getX(), toneStackModelBox.getY() - 20, knobSize, 20);

    channelConfigBox.setBounds(computeXcoord(0, width), computeYcoord(0, height) + 30, 120, 30);
    channelConfigBox.label.setBounds(channelConfigBox.getX(), channelConfigBox.getY() - 20, knobSize, 20);

    brightToggle.setBounds(channelConfigBox.getX(), channelConfigBox.getY() + 40, 120, 30);
}


GainStagesPage::GainStagesPage(Processor &p) :
    stage0LPSlider("STAGE0_LP_SLIDER_LABEL",         "Stage 0 output filter",   this, paramInfos[STAGE0_LP].id.toString(),          p.apvts, " Hz"),
    stage0BypassSlider("STAGE0_BYPASS_SLIDER_LABEL", "Stage 0 cathode bypass",  this, paramInfos[STAGE0_BYPASS].id.toString(),      p.apvts, " dB"),
    stage0BiasSlider("STAGE0_BIAS_SLIDER_LABEL",     "Stage 0 tube bias",       this, paramInfos[STAGE0_BIAS].id.toString(),        p.apvts, ""),

    stage1LPSlider("STAGE1_LP_SLIDER_LABEL",         "Stage 1 output filter",   this, paramInfos[STAGE1_LP].id.toString(),          p.apvts, " Hz"),
    stage1BypassSlider("STAGE1_BYPASS_SLIDER_LABEL", "Stage 1 cathode bypass",  this, paramInfos[STAGE1_BYPASS].id.toString(),      p.apvts, " dB"),
    stage1BiasSlider("STAGE1_BIAS_SLIDER_LABEL",     "Stage 1 tube bias",       this, paramInfos[STAGE1_BIAS].id.toString(),        p.apvts, ""),
    stage1AttSlider("STAGE1_ATTEN_SLIDER_LABEL",     "Stage 1 attenuation",     this, paramInfos[STAGE1_ATTENUATION].id.toString(), p.apvts, ""),

    stage2LPSlider("STAGE2_LP_SLIDER_LABEL",         "Stage 2 output filter",   this, paramInfos[STAGE2_LP].id.toString(),          p.apvts, " Hz"),
    stage2BypassSlider("STAGE2_BYPASS_SLIDER_LABEL", "Stage 2 cathode bypass",  this, paramInfos[STAGE2_BYPASS].id.toString(),      p.apvts, " dB"),
    stage2BiasSlider("STAGE2_BIAS_SLIDER_LABEL",     "Stage 2 tube bias",       this, paramInfos[STAGE2_BIAS].id.toString(),        p.apvts, ""),
    stage2AttSlider("STAGE2_ATTEN_SLIDER_LABEL",     "Stage 2 attenuation",     this, paramInfos[STAGE2_ATTENUATION].id.toString(), p.apvts, ""),

    stage3LPSlider("STAGE3_LP_SLIDER_LABEL",         "Stage 3 output filter",   this, paramInfos[STAGE3_LP].id.toString(),          p.apvts, " Hz"),
    stage3BypassSlider("STAGE3_BYPASS_SLIDER_LABEL", "Stage 3 cathode bypass",  this, paramInfos[STAGE3_BYPASS].id.toString(),      p.apvts, " dB"),
    stage3BiasSlider("STAGE3_BIAS_SLIDER_LABEL",     "Stage 3 tube bias",       this, paramInfos[STAGE3_BIAS].id.toString(),        p.apvts, ""),
    stage3AttSlider("STAGE3_ATTEN_SLIDER_LABEL",     "Stage 3 attenuation",     this, paramInfos[STAGE3_ATTENUATION].id.toString(), p.apvts, ""),

    stage4LPSlider("STAGE4_LP_SLIDER_LABEL",         "Stage 4 output filter",   this, paramInfos[STAGE4_LP].id.toString(),          p.apvts, " Hz"),
    stage4BypassSlider("STAGE4_BYPASS_SLIDER_LABEL", "Stage 4 cathode bypass",  this, paramInfos[STAGE4_BYPASS].id.toString(),      p.apvts, " dB"),
    stage4BiasSlider("STAGE4_BIAS_SLIDER_LABEL",     "Stage 4 tube bias",       this, paramInfos[STAGE4_BIAS].id.toString(),        p.apvts, "")
{
    resetButton.onClick = [&p, this]() {
    
        HSlider* slider_refs[19] = { 
            &stage0LPSlider, &stage0BypassSlider, &stage0BiasSlider,
            &stage1LPSlider, &stage1BypassSlider, &stage1BiasSlider, &stage1AttSlider,
            &stage2LPSlider, &stage2BypassSlider, &stage2BiasSlider, &stage2AttSlider,
            &stage3LPSlider, &stage3BypassSlider, &stage3BiasSlider, &stage3AttSlider,
            &stage4LPSlider, &stage4BypassSlider, &stage4BiasSlider
        };
    
        for (int paramIndex = STAGE0_LP; paramIndex <= STAGE4_BIAS; paramIndex++) {
            p.apvts.getParameter(paramInfos[paramIndex].id.toString())->setValueNotifyingHost(paramInfos[paramIndex].defaultValue);        
            slider_refs[paramIndex - STAGE0_LP]->setValue(paramInfos[paramIndex].defaultValue);
        }
    };
    
    addAndMakeVisible(resetButton);
}

void GainStagesPage::resized() {
    ZoneScoped;

    static const int horizontalSpacing = 25;
    static const int verticalSpacing = 40;
    static const int sliderWidth = 150;
    static const int sliderHeight = 50;

    stage0LPSlider.setBounds(horizontalMargin, verticalMargin, sliderWidth, sliderHeight);
    stage0LPSlider.label.setBounds(stage0LPSlider.getX(), stage0LPSlider.getY() - 15, sliderWidth, 20);

    stage0BypassSlider.setBounds(stage0LPSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage0BypassSlider.label.setBounds(stage0BypassSlider.getX(), stage0BypassSlider.getY() - 15, sliderWidth, 20);

    stage0BiasSlider.setBounds(stage0BypassSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage0BiasSlider.label.setBounds(stage0BiasSlider.getX(), stage0BiasSlider.getY() - 15, sliderWidth, 20);

    resetButton.setBounds(stage0BiasSlider.getX() + 35, stage0BiasSlider.getBottom() + verticalSpacing, 100, 30);


    stage1LPSlider.setBounds(stage0LPSlider.getBounds() + juce::Point(horizontalSpacing + sliderWidth, 0));
    stage1LPSlider.label.setBounds(stage1LPSlider.getX(), stage1LPSlider.getY() - 15, sliderWidth, 20);

    stage1BypassSlider.setBounds(stage1LPSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage1BypassSlider.label.setBounds(stage1BypassSlider.getX(), stage1BypassSlider.getY() - 15, sliderWidth, 20);

    stage1BiasSlider.setBounds(stage1BypassSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage1BiasSlider.label.setBounds(stage1BiasSlider.getX(), stage1BiasSlider.getY() - 15, sliderWidth, 20);

    stage1AttSlider.setBounds(stage1BiasSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage1AttSlider.label.setBounds(stage1AttSlider.getX(), stage1AttSlider.getY() - 15, sliderWidth, 20);


    stage2LPSlider.setBounds(stage1LPSlider.getBounds() + juce::Point(horizontalSpacing + sliderWidth, 0));
    stage2LPSlider.label.setBounds(stage2LPSlider.getX(), stage2LPSlider.getY() - 15, sliderWidth, 20);

    stage2BypassSlider.setBounds(stage2LPSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage2BypassSlider.label.setBounds(stage2BypassSlider.getX(), stage2BypassSlider.getY() - 15, sliderWidth, 20);

    stage2BiasSlider.setBounds(stage2BypassSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage2BiasSlider.label.setBounds(stage2BiasSlider.getX(), stage2BiasSlider.getY() - 15, sliderWidth, 20);

    stage2AttSlider.setBounds(stage2BiasSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage2AttSlider.label.setBounds(stage2AttSlider.getX(), stage2AttSlider.getY() - 15, sliderWidth, 20);


    stage3LPSlider.setBounds(stage2LPSlider.getBounds() + juce::Point(horizontalSpacing + sliderWidth, 0));
    stage3LPSlider.label.setBounds(stage3LPSlider.getX(), stage3LPSlider.getY() - 15, sliderWidth, 20);

    stage3BypassSlider.setBounds(stage3LPSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage3BypassSlider.label.setBounds(stage3BypassSlider.getX(), stage3BypassSlider.getY() - 15, sliderWidth, 20);

    stage3BiasSlider.setBounds(stage3BypassSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage3BiasSlider.label.setBounds(stage3BiasSlider.getX(), stage3BiasSlider.getY() - 15, sliderWidth, 20);

    stage3AttSlider.setBounds(stage3BiasSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage3AttSlider.label.setBounds(stage3AttSlider.getX(), stage3AttSlider.getY() - 15, sliderWidth, 20);


    stage4LPSlider.setBounds(stage3LPSlider.getBounds() + juce::Point(horizontalSpacing + sliderWidth, 0));
    stage4LPSlider.label.setBounds(stage4LPSlider.getX(), stage4LPSlider.getY() - 15, sliderWidth, 20);

    stage4BypassSlider.setBounds(stage4LPSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage4BypassSlider.label.setBounds(stage4BypassSlider.getX(), stage4BypassSlider.getY() - 15, sliderWidth, 20);

    stage4BiasSlider.setBounds(stage4BypassSlider.getBounds() + juce::Point(0, sliderHeight + verticalSpacing));
    stage4BiasSlider.label.setBounds(stage4BiasSlider.getX(), stage4BiasSlider.getY() - 15, sliderWidth, 20);
}

IRLoaderPage::IRLoaderPage(Processor &audioProcessor) {

    bypassButtonAttachment = std::make_unique<ButtonAttachment>(
        audioProcessor.apvts, paramInfos[IR_ACTIVE].id.toString(), bypassToggle
    );

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
        audioProcessor.irLoader.defaultIR = false;
        IRLoaderError error = audioProcessor.irLoader.loadIR();

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
        IRLoaderError error = audioProcessor.irLoader.loadIR();

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
        IRLoaderError error = audioProcessor.irLoader.loadIR();

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

    defaultIRButton.onClick = [&audioProcessor, this]() {
        audioProcessor.irLoader.defaultIR = true;
        IRLoaderError error = audioProcessor.irLoader.loadIR();

        if (error == IRLoaderError::Error) { return; }

        irNameLabel.setText(defaultIRText,
                            juce::NotificationType::dontSendNotification);

        audioProcessor.valueTree.setProperty(irPath1, "", nullptr);
    };

    addAndMakeVisible(defaultIRButton);

    bypassToggle.setToggleState(audioProcessor.irLoader.active,
                                        juce::NotificationType::dontSendNotification);
    bypassToggle.onClick = [&audioProcessor, this]() {
        audioProcessor.irLoader.active = bypassToggle.getToggleState();
    };

    addAndMakeVisible(bypassToggle);
}

void IRLoaderPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();

    irLoadButton.setBounds(computeXcoord(0, width), computeYcoord(0, height), 100, 50);

    bypassToggle.setBounds(computeXcoord(1, width), computeYcoord(0, height) - 20, 120, 30);
    defaultIRButton.setBounds(bypassToggle.getX(), bypassToggle.getY() + 40, 120, 30);

    irNameLabel.setBounds(irLoadButton.getX(), irLoadButton.getY() + irLoadButton.getHeight() + 5, 
                          400, 50);

    nextIRButton.setBounds(computeXcoord(0, width), computeYcoord(1, height), 120, 30);
    prevIRButton.setBounds(nextIRButton.getX(), nextIRButton.getY() + 50, 120, 30);
}

EQPage::EQPage(Processor &p) :
    lowcutFreqKnob("LOW_CUT_FREQ_KNOB_LABEL",       "Low cut",         this, paramInfos[LOW_CUT_FREQ].id.toString(),    p.apvts, " Hz"),
    lowShelfFreqKnob("LOW_SHELF_FREQ_KNOB_LABEL",   "Low Shelf Freq",  this, paramInfos[LOW_SHELF_FREQ].id.toString(),  p.apvts, " Hz"),
    lowShelfGainKnob("LOW_SHELF_GAIN_KNOB_LABEL",   "Gain",            this, paramInfos[LOW_SHELF_GAIN].id.toString(),  p.apvts, " dB"),
    lowMidFreqKnob("LOWMID_FREQ_KNOB_LABEL",        "Freq",            this, paramInfos[LOWMID_FREQ].id.toString(),     p.apvts, " Hz"),
    lowMidGainKnob("LOWMID_GAIN_KNOB_LABEL",        "Gain",            this, paramInfos[LOWMID_GAIN].id.toString(),     p.apvts, " dB"),
    lowMidQKnob("LOWMID_Q_KNOB_LABEL",              "Q",               this, paramInfos[LOWMID_Q].id.toString(),        p.apvts, ""),
    midFreqKnob("MID_FREQ_KNOB_LABEL",              "Freq",            this, paramInfos[MID_FREQ].id.toString(),        p.apvts, " Hz"),
    midGainKnob("MID_GAIN_KNOB_LABEL",              "Gain",            this, paramInfos[MID_GAIN].id.toString(),        p.apvts, " dB"),
    midQKnob("MID_Q_KNOB_LABEL",                    "Q",               this, paramInfos[MID_Q].id.toString(),           p.apvts, ""),
    highFreqKnob("HIGH_FREQ_KNOB_LABEL",            "Freq",            this, paramInfos[HIGH_FREQ].id.toString(),       p.apvts, " Hz"),
    highGainKnob("HIGH_GAIN_KNOB_LABEL",            "Gain",            this, paramInfos[HIGH_GAIN].id.toString(),       p.apvts, " dB"),
    highQKnob("HIGH_Q_KNOB_LABEL",                  "Q",               this, paramInfos[HIGH_Q].id.toString(),          p.apvts, ""),
    highShelfFreqKnob("HIGH_SHELF_FREQ_KNOB_LABEL", "High Shelf Freq", this, paramInfos[HIGH_SHELF_FREQ].id.toString(), p.apvts, " Hz"),
    highShelfGainKnob("HIGH_SHELF_GAIN_KNOB_LABEL", "Gain",            this, paramInfos[HIGH_SHELF_GAIN].id.toString(), p.apvts, " dB"),
    highcutFreqKnob("HIGH_CUT_FREQ_KNOB_LABEL",     "High cut",        this, paramInfos[HIGH_CUT_FREQ].id.toString(),   p.apvts, " Hz")
{
    resetButton.onClick = [&p, this]() {
    
        p.apvts.getParameter(paramInfos[LOW_SHELF_GAIN].id.toString())->setValueNotifyingHost(paramInfos[LOW_SHELF_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[LOWMID_GAIN].id.toString())->setValueNotifyingHost(paramInfos[LOWMID_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[MID_GAIN].id.toString())->setValueNotifyingHost(paramInfos[MID_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[HIGH_GAIN].id.toString())->setValueNotifyingHost(paramInfos[HIGH_GAIN].defaultValue);
        p.apvts.getParameter(paramInfos[HIGH_SHELF_GAIN].id.toString())->setValueNotifyingHost(paramInfos[HIGH_SHELF_GAIN].defaultValue);
    
        lowShelfGainKnob.setValue(paramInfos[LOW_SHELF_GAIN].defaultValue);
        lowMidGainKnob.setValue(paramInfos[LOWMID_GAIN].defaultValue);
        midGainKnob.setValue(paramInfos[MID_GAIN].defaultValue);
        highGainKnob.setValue(paramInfos[HIGH_GAIN].defaultValue);
        highShelfGainKnob.setValue(paramInfos[HIGH_SHELF_GAIN].defaultValue);
    };

    addAndMakeVisible(resetButton);
}

void EQPage::resized() {
    ZoneScoped;

    int width = getWidth();
    int height = getHeight();

    static const int eqKnobSize = 90;

    resetButton.setBounds(computeXcoord(0, width), computeYcoord(0, height), 100, 30);
    
    lowShelfFreqKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    lowShelfFreqKnob.label.setBounds(lowShelfFreqKnob.getX(), lowShelfFreqKnob.getY() - 15, eqKnobSize, 20);
    lowShelfGainKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    lowShelfGainKnob.label.setBounds(lowShelfGainKnob.getX(), lowShelfGainKnob.getY() - 15, eqKnobSize, 20);

    lowcutFreqKnob.setBounds(computeXcoord(1, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    lowcutFreqKnob.label.setBounds(lowcutFreqKnob.getX(), lowcutFreqKnob.getY() - 15, eqKnobSize, 20);

    lowMidFreqKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    lowMidFreqKnob.label.setBounds(lowMidFreqKnob.getX(), lowMidFreqKnob.getY() - 15, eqKnobSize, 20);
    lowMidGainKnob.setBounds(computeXcoord(2, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    lowMidGainKnob.label.setBounds(lowMidGainKnob.getX(), lowMidGainKnob.getY() - 15, eqKnobSize, 20);
    lowMidQKnob.setBounds(computeXcoord(2, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    lowMidQKnob.label.setBounds(lowMidQKnob.getX(), lowMidQKnob.getY() - 15, eqKnobSize, 20);

    midFreqKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    midFreqKnob.label.setBounds(midFreqKnob.getX(), midFreqKnob.getY() - 15, eqKnobSize, 20);
    midGainKnob.setBounds(computeXcoord(3, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    midGainKnob.label.setBounds(midGainKnob.getX(), midGainKnob.getY() - 15, eqKnobSize, 20);
    midQKnob.setBounds(computeXcoord(3, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    midQKnob.label.setBounds(midQKnob.getX(), midQKnob.getY() - 15, eqKnobSize, 20);

    highFreqKnob.setBounds(computeXcoord(4, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    highFreqKnob.label.setBounds(highFreqKnob.getX(), highFreqKnob.getY() - 15, eqKnobSize, 20);
    highGainKnob.setBounds(computeXcoord(4, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    highGainKnob.label.setBounds(highGainKnob.getX(), highGainKnob.getY() - 15, eqKnobSize, 20);
    highQKnob.setBounds(computeXcoord(4, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    highQKnob.label.setBounds(highQKnob.getX(), highQKnob.getY() - 15, eqKnobSize, 20);

    highShelfFreqKnob.setBounds(computeXcoord(5, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    highShelfFreqKnob.label.setBounds(highShelfFreqKnob.getX(), highShelfFreqKnob.getY() - 15, eqKnobSize, 20);
    highShelfGainKnob.setBounds(computeXcoord(5, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    highShelfGainKnob.label.setBounds(highShelfGainKnob.getX(), highShelfGainKnob.getY() - 15, eqKnobSize, 20);

    highcutFreqKnob.setBounds(computeXcoord(5, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    highcutFreqKnob.label.setBounds(highcutFreqKnob.getX(), highcutFreqKnob.getY() - 15, eqKnobSize, 20);
}


//==============================================================================
Editor::Editor (Processor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    gateBoostPage(p),
    ampPage(p),
    gainStagesPage(p),
    irLoaderPage(p),
    eqPage(p),
    volumePanel(p)
{

    juce::Colour tabColour = juce::Colours::black;
    tabs.setColour(juce::TabbedComponent::ColourIds::backgroundColourId, tabColour);
    tabs.setColour(juce::TabbedComponent::ColourIds::outlineColourId, tabColour);
    
    tabs.addTab("Input",       tabColour, &gateBoostPage,  true);
    tabs.addTab("Amplifier",   tabColour, &ampPage,        true);
    tabs.addTab("Gain Stages", tabColour, &gainStagesPage, true);
    tabs.addTab("EQ",          tabColour, &eqPage,         true);
    tabs.addTab("IR Loader",   tabColour, &irLoaderPage,   true);
    tabs.setCurrentTabIndex(1);
    
    // passer un ptr vers gainStagesPage dans ampPage pour 
    // ampPage.gainstages = &gainStagesPage;
    
    addAndMakeVisible(tabs);
    addAndMakeVisible(volumePanel);

    // getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::darkred);
    setLookAndFeel(&this->lookAndFeel);
    setSize(windoWidth, windowHeight);
    setResizable(false, false);
}

Editor::~Editor() {
    setLookAndFeel(nullptr);
}

//==============================================================================
void Editor::paint (juce::Graphics& g) {
    ZoneScoped;
	g.fillAll(backgroundColor);
    volumePanel.paint(g);
}

void Editor::resized() {
    ZoneScoped;

    juce::Rectangle<int> bounds = getLocalBounds();

    volumePanel.setBounds(bounds.removeFromRight(100));

    gateBoostPage.setBounds(bounds);
    ampPage.setBounds(bounds);
    eqPage.setBounds(bounds);
    irLoaderPage.setBounds(bounds);

    tabs.setBounds(bounds);

    gateBoostPage.resized();
    ampPage.resized();
    gainStagesPage.resized();
    irLoaderPage.resized();
    eqPage.resized();
}


Knob::Knob(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix)
: label{labelID, name}
{
    setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    setTextValueSuffix(suffix);
    comp->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setColour(juce::Label::ColourIds::outlineColourId, juce::Colours::transparentBlack);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);

    sliderAttachment = std::make_unique<SliderAttachment>(
        apvts, paramID, *this
    );
}

VSlider::VSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix)
: label{labelID, name}
{
    setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    setTextValueSuffix(suffix);
    comp->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);


    sliderAttachment = std::make_unique<SliderAttachment>(
        apvts, paramID, *this
    );
}

HSlider::HSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String parameterID, Apvts &apvts, juce::String suffix)
: label{labelID, name}
{
    setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    setTextValueSuffix(suffix);
    comp->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);

    sliderAttachment = std::make_unique<SliderAttachment>(
        apvts, parameterID, *this
    );
    
    paramID = parameterID;
}

ComboBox::ComboBox(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts)
         : label {labelID, name}
{
    setEditableText(false);
    setJustificationType(juce::Justification::centred);
    comp->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);

    comp->addAndMakeVisible(label);

    boxAttachment = std::make_unique<ComboBoxAttachment>(
        apvts, paramID, *this
    );
}
