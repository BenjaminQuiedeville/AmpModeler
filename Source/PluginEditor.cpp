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


GateBoostPage::GateBoostPage(Processor &p) :
    gateKnob("GATE_KNOB_LABEL",                "Gate Thresh",     this, ParamIDs[GATE_THRESH].toString(),     p.apvts, " dB"),
    // gateAttackKnob("GATE_Attack_LABEL",        "Gate Attack",     this, ParamIDs[GATE_ATTACK].toString(),     p.apvts, " ms"),
    // gateReleaseKnob("GATE_RELEASE_LABEL",      "Gate Release",    this, ParamIDs[GATE_RELEASE].toString(),    p.apvts, " ms"),
    // gateReturnKnob("GATE_RETURN_LABEL",        "Gate Hysteresis", this, ParamIDs[GATE_RETURN].toString(),     p.apvts, " dB"),

    boostAttackKnob("BOOST_ATTACK_KNOB_LABEL", "Boost Top",       this, ParamIDs[SCREAMER_AMOUNT].toString(), p.apvts, " dB"),
    boostFreqKnob("BOOST_FREQ_KNOB_LABEL",     "Boost Freq",      this, ParamIDs[SCREAMER_FREQ].toString(),   p.apvts, " Hz"),
    boostTightKnob("BOOST_TIGHT_KNOB_LABEL",   "Boost Tight",     this, ParamIDs[TIGHT].toString(),           p.apvts, " Hz"),
    inputGainKnob("INPUT_GAIN_KNOB_LABEL",     "Input Gain",      this, ParamIDs[INPUT_GAIN].toString(),      p.apvts, " dB")

{
    gateToggle.setToggleState(p.gateActive,
                              juce::NotificationType::dontSendNotification);

    preampToggle.setToggleState(p.preampActive,
                              juce::NotificationType::dontSendNotification);

    tonestackToggle.setToggleState(p.tonestackActive,
                              juce::NotificationType::dontSendNotification);


    gateToggle.onClick = [&p, this]() { p.gateActive = gateToggle.getToggleState(); };
    preampToggle.onClick = [&p, this]() { p.preampActive = preampToggle.getToggleState(); };
    tonestackToggle.onClick = [&p, this]() { p.tonestackActive = tonestackToggle.getToggleState(); };

    addAndMakeVisible(gateToggle);
    addAndMakeVisible(preampToggle);
    addAndMakeVisible(tonestackToggle);
}

void GateBoostPage::resized() {

    int width = getWidth();
    int height = getHeight();


    inputGainKnob.setBounds(computeXcoord(0, width), computeYcoord(0, height), knobSize, knobSize);
    inputGainKnob.label.setBounds(inputGainKnob.getX(), inputGainKnob.getY() - 15,
                              inputGainKnob.getWidth(), 20);

    gateKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    gateKnob.label.setBounds(gateKnob.getX(), gateKnob.getY() - 15,
                              gateKnob.getWidth(), 20);


    // gateAttackKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    // gateAttackKnob.label.setBounds(gateAttackKnob.getX(), gateAttackKnob.getY() - 15,
    //                                gateAttackKnob.getWidth(), 20);

    // gateReleaseKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReleaseKnob.label.setBounds(gateReleaseKnob.getX(), gateReleaseKnob.getY() - 15,
    //                                 gateReleaseKnob.getWidth(), 20);

    // gateReturnKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReturnKnob.label.setBounds(gateReturnKnob.getX(), gateReturnKnob.getY() - 15,
    //                                gateReturnKnob.getWidth(), 20);


    boostAttackKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), knobSize, knobSize);
    boostAttackKnob.label.setBounds(boostAttackKnob.getX(), boostAttackKnob.getY() - 15,
                                  boostAttackKnob.getWidth(), 20);

    boostFreqKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), knobSize, knobSize);
    boostFreqKnob.label.setBounds(boostFreqKnob.getX(), boostFreqKnob.getY() - 15,
                                  boostFreqKnob.getWidth(), 20);

    boostTightKnob.setBounds(computeXcoord(2, width), computeYcoord(1, height), knobSize, knobSize);
    boostTightKnob.label.setBounds(boostTightKnob.getX(), boostTightKnob.getY() - 15,
                                    boostTightKnob.getWidth(), 20);

    gateToggle.setBounds(computeXcoord(4, width), computeYcoord(0, height), 200, 30);
    preampToggle.setBounds(gateToggle.getBounds() + juce::Point(0, gateToggle.getHeight() + 20));
    tonestackToggle.setBounds(preampToggle.getBounds() + juce::Point(0, preampToggle.getHeight() + 20));
}


AmplifierPage::AmplifierPage(Processor &p) :
    gainKnob("GAIN_KNOB_LABEL",                  "Pre Gain",        this, ParamIDs[PREAMP_GAIN].toString(),       p.apvts, ""),

    inputFilterKnob("INPUT_KNOB_FILTER_LABEL",   "Input Filter",    this, ParamIDs[INPUT_FILTER].toString(),      p.apvts, " Hz"),

    bassEQKnob("BASS_EQ_KNOB_LABEL",             "Bass",            this, ParamIDs[TONESTACK_BASS].toString(),    p.apvts, ""),
    midEQKnob("MIDDLE_EQ_KNOB_LABEL",            "Mid",             this, ParamIDs[TONESTACK_MIDDLE].toString(),  p.apvts, ""),
    trebbleEQKnob("TREBBLE_EQ_KNOB_LABEL",       "Trebble",         this, ParamIDs[TONESTACK_TREBBLE].toString(), p.apvts, ""),

    preampVolumeKnob("PREMP_VOLUME_KNOB_LABEL",  "Post Gain",       this, ParamIDs[PREAMP_VOLUME].toString(),     p.apvts, " dB"),

    resonanceKnob("RESONANCE_KNOB_LABEL",        "Resonance",       this, ParamIDs[RESONANCE].toString(),         p.apvts, ""),
    presenceKnob("PRESENCE_KNOB_LABEL",          "Presence",        this, ParamIDs[PRESENCE].toString(),          p.apvts, ""),

    ampChannelBox("AMP_CHANNEL_BOX_LABEL",       "Amp Channel",     this, ParamIDs[CHANNEL].toString(),           p.apvts),
    toneStackModelBox("TONE_MODEL_BOX_LABEL",    "Tonestack Model", this, ParamIDs[TONESTACK_MODEL].toString(),   p.apvts),
    channelConfigBox("CHANNEL_CONFIG_BOX_LABEL", "Channel config",  this, ParamIDs[CHANNEL_CONFIG].toString(),    p.apvts)
{
    gainKnob.setNumDecimalPlacesToDisplay(2);

    ampChannelBox.addItemList({"Channel 1", "Channel 2", "Channel 3", "Channel 4"}, 1);
    ampChannelBox.setSelectedId((int)*p.apvts.getRawParameterValue(ParamIDs[CHANNEL]),
                                 juce::NotificationType::dontSendNotification);

    toneStackModelBox.addItemList({"Savage", "JCM", "SLO", "Recto", "Orange"}, 1);
    toneStackModelBox.setSelectedId((int)*p.apvts.getRawParameterValue(ParamIDs[TONESTACK_MODEL]) + 1,
                                     juce::NotificationType::dontSendNotification);

    channelConfigBox.addItemList({"Mono","Fake Stereo", "Stereo"}, 1);
    channelConfigBox.setSelectedId((int)*p.apvts.getRawParameterValue(ParamIDs[CHANNEL_CONFIG]) + 1,
                                    juce::NotificationType::dontSendNotification);
}


void AmplifierPage::resized() {

    int width = getWidth();
    int height = getHeight();

    gainKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    gainKnob.label.setBounds(gainKnob.getX(), gainKnob.getY() - 15,
                              gainKnob.getWidth(), 20);

    inputFilterKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    inputFilterKnob.label.setBounds(inputFilterKnob.getX(), inputFilterKnob.getY() - 15,
                                     inputFilterKnob.getWidth(), 20);

    bassEQKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), knobSize, knobSize);
    bassEQKnob.label.setBounds(bassEQKnob.getX(), bassEQKnob.getY() - 15,
                                bassEQKnob.getWidth(), 20);

    midEQKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), knobSize, knobSize);
    midEQKnob.label.setBounds(midEQKnob.getX(), midEQKnob.getY() - 15,
                               midEQKnob.getWidth(), 20);

    trebbleEQKnob.setBounds(computeXcoord(4, width), computeYcoord(0, height), knobSize, knobSize);
    trebbleEQKnob.label.setBounds(trebbleEQKnob.getX(), trebbleEQKnob.getY() - 15,
                                   trebbleEQKnob.getWidth(), 20);

    resonanceKnob.setBounds(computeXcoord(5, width), computeYcoord(1, height), knobSize, knobSize);
    resonanceKnob.label.setBounds(resonanceKnob.getX(), resonanceKnob.getY() - 15,
                                   resonanceKnob.getWidth(), 20);

    presenceKnob.setBounds(computeXcoord(5, width), computeYcoord(0, height), knobSize, knobSize);
    presenceKnob.label.setBounds(presenceKnob.getX(), presenceKnob.getY() - 15,
                                   presenceKnob.getWidth(), 20);



    preampVolumeKnob.setBounds(computeXcoord(6, width), computeYcoord(0, height), knobSize, knobSize);
    preampVolumeKnob.label.setBounds(preampVolumeKnob.getX(), preampVolumeKnob.getY() - 15,
                                      preampVolumeKnob.getWidth(), 20);


    ampChannelBox.setBounds(computeXcoord(2, width), computeYcoord(1, height) + 30, 120, 30);
    ampChannelBox.label.setBounds(ampChannelBox.getX(), ampChannelBox.getY() - 20,
                                   ampChannelBox.getWidth(), 20);

    toneStackModelBox.setBounds(computeXcoord(3, width), computeYcoord(1, height) + 30, 120, 30);
    toneStackModelBox.label.setBounds(toneStackModelBox.getX(), toneStackModelBox.getY() - 20,
                                       toneStackModelBox.getWidth(), 20);

    channelConfigBox.setBounds(computeXcoord(0, width), computeYcoord(0, height) + 30, 120, 30);
    channelConfigBox.label.setBounds(channelConfigBox.getX(), channelConfigBox.getY() - 20,
                                      channelConfigBox.getWidth(), 20);

}

GainStagesPage::GainStagesPage(Processor &p) :
    stage0LPSlider("STAGE0_LP_SLIDER_LABEL",         "Stage 0 output filter",   this, ParamIDs[STAGE0_LP].toString(),     p.apvts, " Hz"),
    stage0BypassSlider("STAGE0_BYPASS_SLIDER_LABEL", "Stage 0 cathode bypass",  this, ParamIDs[STAGE0_BYPASS].toString(), p.apvts, " dB"),
    stage0BiasSlider("STAGE0_BIAS_SLIDER_LABEL",     "Stage 0 tube bias",       this, ParamIDs[STAGE0_BIAS].toString(),   p.apvts, ""),

    stage1HPSlider("STAGE1_HP_SLIDER_LABEL",         "Stage 1 coupling filter", this, ParamIDs[STAGE1_HP].toString(),     p.apvts, " Hz"),
    stage1LPSlider("STAGE1_LP_SLIDER_LABEL",         "Stage 1 output filter",   this, ParamIDs[STAGE1_LP].toString(),     p.apvts, " Hz"),
    stage1BypassSlider("STAGE1_BYPASS_SLIDER_LABEL", "Stage 1 cathode bypass",  this, ParamIDs[STAGE1_BYPASS].toString(), p.apvts, " dB"),
    stage1BiasSlider("STAGE1_BIAS_SLIDER_LABEL",     "Stage 1 tube bias",       this, ParamIDs[STAGE1_BIAS].toString(),   p.apvts, ""),

    stage2HPSlider("STAGE2_HP_SLIDER_LABEL",         "Stage 2 coupling filter", this, ParamIDs[STAGE2_HP].toString(),     p.apvts, " Hz"),
    stage2LPSlider("STAGE2_LP_SLIDER_LABEL",         "Stage 2 output filter",   this, ParamIDs[STAGE2_LP].toString(),     p.apvts, " Hz"),
    stage2BypassSlider("STAGE2_BYPASS_SLIDER_LABEL", "Stage 2 cathode bypass",  this, ParamIDs[STAGE2_BYPASS].toString(), p.apvts, " dB"),
    stage2BiasSlider("STAGE2_BIAS_SLIDER_LABEL",     "Stage 2 tube bias",       this, ParamIDs[STAGE2_BIAS].toString(),   p.apvts, ""),

    stage3HPSlider("STAGE3_HP_SLIDER_LABEL",         "Stage 3 coupling filter", this, ParamIDs[STAGE3_HP].toString(),     p.apvts, " Hz"),
    stage3LPSlider("STAGE3_LP_SLIDER_LABEL",         "Stage 3 output filter",   this, ParamIDs[STAGE3_LP].toString(),     p.apvts, " Hz"),
    stage3BypassSlider("STAGE3_BYPASS_SLIDER_LABEL", "Stage 3 cathode bypass",  this, ParamIDs[STAGE3_BYPASS].toString(), p.apvts, " dB"),
    stage3BiasSlider("STAGE3_BIAS_SLIDER_LABEL",     "Stage 3 tube bias",       this, ParamIDs[STAGE3_BIAS].toString(),   p.apvts, ""),

    stage4HPSlider("STAGE4_HP_SLIDER_LABEL",         "Stage 4 coupling filter", this, ParamIDs[STAGE4_HP].toString(),     p.apvts, " Hz"),
    stage4LPSlider("STAGE4_LP_SLIDER_LABEL",         "Stage 4 output filter",   this, ParamIDs[STAGE4_LP].toString(),     p.apvts, " Hz"),
    stage4BypassSlider("STAGE4_BYPASS_SLIDER_LABEL", "Stage 4 cathode bypass",  this, ParamIDs[STAGE4_BYPASS].toString(), p.apvts, " dB"),
    stage4BiasSlider("STAGE4_BIAS_SLIDER_LABEL",     "Stage 4 tube bias",       this, ParamIDs[STAGE4_BIAS].toString(),   p.apvts, "")
{

    resetButton.onClick = [&p, this]() {
    
        HSlider* slider_refs[19] = { &stage0LPSlider, &stage0BypassSlider, &stage0BiasSlider, 
                    &stage1HPSlider, &stage1LPSlider, &stage1BypassSlider, &stage1BiasSlider, 
                    &stage2HPSlider, &stage2LPSlider, &stage2BypassSlider, &stage2BiasSlider, 
                    &stage3HPSlider, &stage3LPSlider, &stage3BypassSlider, &stage3BiasSlider, 
                    &stage4HPSlider, &stage4LPSlider, &stage4BypassSlider, &stage4BiasSlider
        };
    
        for (int paramIndex = STAGE0_LP; paramIndex <= STAGE4_BIAS; paramIndex++) {
            p.apvts.getParameter(ParamIDs[paramIndex].toString())->setValueNotifyingHost(defaultParamValues[paramIndex]);        
            slider_refs[paramIndex - STAGE0_LP]->setValue(defaultParamValues[paramIndex]);
        }
    };
    
    addAndMakeVisible(resetButton);
}

void GainStagesPage::resized() {

    static const int verticalSpacing = 30;
    static const int horizontalSpacing = 20;

    resetButton.setBounds(horizontalMargin, verticalMargin, 100, 30);

    stage0LPSlider.setBounds(horizontalMargin, verticalMargin + 50 + verticalSpacing, 150, 50);
    stage0LPSlider.label.setBounds(stage0LPSlider.getX(), stage0LPSlider.getY() - 15,
                                        stage0LPSlider.getWidth(), 20);

    stage0BypassSlider.setBounds(stage0LPSlider.getBounds() + juce::Point(0, stage0LPSlider.getHeight() + verticalSpacing));
    stage0BypassSlider.label.setBounds(stage0BypassSlider.getX(), stage0BypassSlider.getY() - 15,
                                        stage0BypassSlider.getWidth(), 20);

    stage0BiasSlider.setBounds(stage0BypassSlider.getBounds() + juce::Point(0, stage0BypassSlider.getHeight() + verticalSpacing));
    stage0BiasSlider.label.setBounds(stage0BiasSlider.getX(), stage0BiasSlider.getY() - 15,
                                        stage0BiasSlider.getWidth(), 20);


    stage1HPSlider.setBounds(stage0LPSlider.getRight() + horizontalSpacing, verticalMargin, stage0LPSlider.getWidth(), stage0LPSlider.getHeight());
    stage1HPSlider.label.setBounds(stage1HPSlider.getX(), stage1HPSlider.getY() - 15,
                                        stage1HPSlider.getWidth(), 20);

    stage1LPSlider.setBounds(stage1HPSlider.getBounds() + juce::Point(0, stage1HPSlider.getHeight() + verticalSpacing));
    stage1LPSlider.label.setBounds(stage1LPSlider.getX(), stage1LPSlider.getY() - 15,
                                        stage1LPSlider.getWidth(), 20);

    stage1BypassSlider.setBounds(stage1LPSlider.getBounds() + juce::Point(0, stage1LPSlider.getHeight() + verticalSpacing));
    stage1BypassSlider.label.setBounds(stage1BypassSlider.getX(), stage1BypassSlider.getY() - 15,
                                        stage1BypassSlider.getWidth(), 20);

    stage1BiasSlider.setBounds(stage1BypassSlider.getBounds() + juce::Point(0, stage1BypassSlider.getHeight() + verticalSpacing));
    stage1BiasSlider.label.setBounds(stage1BiasSlider.getX(), stage1BiasSlider.getY() - 15,
                                        stage1BiasSlider.getWidth(), 20);


    stage2HPSlider.setBounds(stage1LPSlider.getRight() + horizontalSpacing, verticalMargin, stage1LPSlider.getWidth(), stage1LPSlider.getHeight());
    stage2HPSlider.label.setBounds(stage2HPSlider.getX(), stage2HPSlider.getY() - 15,
                                        stage2HPSlider.getWidth(), 20);

    stage2LPSlider.setBounds(stage2HPSlider.getBounds() + juce::Point(0, stage2HPSlider.getHeight() + verticalSpacing));
    stage2LPSlider.label.setBounds(stage2LPSlider.getX(), stage2LPSlider.getY() - 15,
                                        stage2LPSlider.getWidth(), 20);

    stage2BypassSlider.setBounds(stage2LPSlider.getBounds() + juce::Point(0, stage2LPSlider.getHeight() + verticalSpacing));
    stage2BypassSlider.label.setBounds(stage2BypassSlider.getX(), stage2BypassSlider.getY() - 15,
                                        stage2BypassSlider.getWidth(), 20);

    stage2BiasSlider.setBounds(stage2BypassSlider.getBounds() + juce::Point(0, stage2BypassSlider.getHeight() + verticalSpacing));
    stage2BiasSlider.label.setBounds(stage2BiasSlider.getX(), stage2BiasSlider.getY() - 15,
                                        stage2BiasSlider.getWidth(), 20);



    stage3HPSlider.setBounds(stage2LPSlider.getRight() + horizontalSpacing, verticalMargin, stage2LPSlider.getWidth(), stage2LPSlider.getHeight());
    stage3HPSlider.label.setBounds(stage3HPSlider.getX(), stage3HPSlider.getY() - 15,
                                        stage3HPSlider.getWidth(), 20);

    stage3LPSlider.setBounds(stage3HPSlider.getBounds() + juce::Point(0, stage3HPSlider.getHeight() + verticalSpacing));
    stage3LPSlider.label.setBounds(stage3LPSlider.getX(), stage3LPSlider.getY() - 15,
                                        stage3LPSlider.getWidth(), 20);

    stage3BypassSlider.setBounds(stage3LPSlider.getBounds() + juce::Point(0, stage3LPSlider.getHeight() + verticalSpacing));
    stage3BypassSlider.label.setBounds(stage3BypassSlider.getX(), stage3BypassSlider.getY() - 15,
                                        stage3BypassSlider.getWidth(), 20);

    stage3BiasSlider.setBounds(stage3BypassSlider.getBounds() + juce::Point(0, stage3BypassSlider.getHeight() + verticalSpacing));
    stage3BiasSlider.label.setBounds(stage3BiasSlider.getX(), stage3BiasSlider.getY() - 15,
                                        stage3BiasSlider.getWidth(), 20);


    stage4HPSlider.setBounds(stage3LPSlider.getRight() + horizontalSpacing, verticalMargin, stage3LPSlider.getWidth(), stage3LPSlider.getHeight());
    stage4HPSlider.label.setBounds(stage4HPSlider.getX(), stage4HPSlider.getY() - 15,
                                        stage4HPSlider.getWidth(), 20);

    stage4LPSlider.setBounds(stage4HPSlider.getBounds() + juce::Point(0, stage4HPSlider.getHeight() + verticalSpacing));
    stage4LPSlider.label.setBounds(stage4LPSlider.getX(), stage4LPSlider.getY() - 15,
                                        stage4LPSlider.getWidth(), 20);

    stage4BypassSlider.setBounds(stage4LPSlider.getBounds() + juce::Point(0, stage4LPSlider.getHeight() + verticalSpacing));
    stage4BypassSlider.label.setBounds(stage4BypassSlider.getX(), stage4BypassSlider.getY() - 15,
                                        stage4BypassSlider.getWidth(), 20);

    stage4BiasSlider.setBounds(stage4BypassSlider.getBounds() + juce::Point(0, stage4BypassSlider.getHeight() + verticalSpacing));
    stage4BiasSlider.label.setBounds(stage4BiasSlider.getX(), stage4BiasSlider.getY() - 15,
                                        stage4BiasSlider.getWidth(), 20);
}

IRLoaderPage::IRLoaderPage(Processor &audioProcessor) {

    bypassButtonAttachment = std::make_unique<ButtonAttachment>(
        audioProcessor.apvts, ParamIDs[BYPASS_IR].toString(), bypassToggle
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

    int width = getWidth();
    int height = getHeight();

    irLoadButton.setBounds(computeXcoord(0, width), computeYcoord(0, height), 100, 50);

    bypassToggle.setBounds(computeXcoord(1, width), computeYcoord(0, height) - 20, 120, 30);
    defaultIRButton.setBounds(bypassToggle.getX(), bypassToggle.getY() + 40,
                                      120, 30);

    irNameLabel.setBounds(irLoadButton.getX(),
                          irLoadButton.getY() + irLoadButton.getHeight() + 5,
                          400, 50);

    nextIRButton.setBounds(computeXcoord(0, width), computeYcoord(1, height), 120, 30);
    prevIRButton.setBounds(nextIRButton.getX(), nextIRButton.getY() + 50, 120, 30);
}

EQPage::EQPage(Processor &p) :
    lowcutFreqKnob("LOW_CUT_FREQ_KNOB_LABEL",       "Low cut",         this, ParamIDs[LOW_CUT_FREQ].toString(),    p.apvts, " Hz"),
    lowShelfFreqKnob("LOW_SHELF_FREQ_KNOB_LABEL",   "Low Shelf Freq",  this, ParamIDs[LOW_SHELF_FREQ].toString(),  p.apvts, " Hz"),
    lowShelfGainKnob("LOW_SHELF_GAIN_KNOB_LABEL",   "Gain",            this, ParamIDs[LOW_SHELF_GAIN].toString(),  p.apvts, " dB"),
    lowMidFreqKnob("LOWMID_FREQ_KNOB_LABEL",        "Freq",            this, ParamIDs[LOWMID_FREQ].toString(),     p.apvts, " Hz"),
    lowMidGainKnob("LOWMID_GAIN_KNOB_LABEL",        "Gain",            this, ParamIDs[LOWMID_GAIN].toString(),     p.apvts, " dB"),
    lowMidQKnob("LOWMID_Q_KNOB_LABEL",              "Q",               this, ParamIDs[LOWMID_Q].toString(),        p.apvts, ""),
    midFreqKnob("MID_FREQ_KNOB_LABEL",              "Freq",            this, ParamIDs[MID_FREQ].toString(),        p.apvts, " Hz"),
    midGainKnob("MID_GAIN_KNOB_LABEL",              "Gain",            this, ParamIDs[MID_GAIN].toString(),        p.apvts, " dB"),
    midQKnob("MID_Q_KNOB_LABEL",                    "Q",               this, ParamIDs[MID_Q].toString(),           p.apvts, ""),
    highFreqKnob("HIGH_FREQ_KNOB_LABEL",            "Freq",            this, ParamIDs[HIGH_FREQ].toString(),       p.apvts, " Hz"),
    highGainKnob("HIGH_GAIN_KNOB_LABEL",            "Gain",            this, ParamIDs[HIGH_GAIN].toString(),       p.apvts, " dB"),
    highQKnob("HIGH_Q_KNOB_LABEL",                  "Q",               this, ParamIDs[HIGH_Q].toString(),          p.apvts, ""),
    highShelfFreqKnob("HIGH_SHELF_FREQ_KNOB_LABEL", "High Shelf Freq", this, ParamIDs[HIGH_SHELF_FREQ].toString(), p.apvts, " Hz"),
    highShelfGainKnob("HIGH_SHELF_GAIN_KNOB_LABEL", "Gain",            this, ParamIDs[HIGH_SHELF_GAIN].toString(), p.apvts, " dB"),
    highcutFreqKnob("HIGH_CUT_FREQ_KNOB_LABEL",     "High cut",        this, ParamIDs[HIGH_CUT_FREQ].toString(),   p.apvts, " Hz")
{

    resetButton.onClick = [&p, this]() {
    
        p.apvts.getParameter(ParamIDs[LOW_SHELF_GAIN].toString())->setValueNotifyingHost(defaultParamValues[LOW_SHELF_GAIN]);
        p.apvts.getParameter(ParamIDs[LOWMID_GAIN].toString())->setValueNotifyingHost(defaultParamValues[LOWMID_GAIN]);
        p.apvts.getParameter(ParamIDs[MID_GAIN].toString())->setValueNotifyingHost(defaultParamValues[MID_GAIN]);
        p.apvts.getParameter(ParamIDs[HIGH_GAIN].toString())->setValueNotifyingHost(defaultParamValues[HIGH_GAIN]);
        p.apvts.getParameter(ParamIDs[HIGH_SHELF_GAIN].toString())->setValueNotifyingHost(defaultParamValues[HIGH_SHELF_GAIN]);
    
        lowShelfGainKnob.setValue(defaultParamValues[LOW_SHELF_GAIN]);
        lowMidGainKnob.setValue(defaultParamValues[LOWMID_GAIN]);
        midGainKnob.setValue(defaultParamValues[MID_GAIN]);
        highGainKnob.setValue(defaultParamValues[HIGH_GAIN]);
        highShelfGainKnob.setValue(defaultParamValues[HIGH_SHELF_GAIN]);
    };

    addAndMakeVisible(resetButton);
}

void EQPage::resized() {

    // resetButton

    int width = getWidth();
    int height = getHeight();

    static const int eqKnobSize = 90;

    resetButton.setBounds(computeXcoord(0, width), computeYcoord(0, height), 100, 30);
    
    lowShelfFreqKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    lowShelfFreqKnob.label.setBounds(lowShelfFreqKnob.getX(), lowShelfFreqKnob.getY() - 15,
                             lowShelfFreqKnob.getWidth(), 20);
    lowShelfGainKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    lowShelfGainKnob.label.setBounds(lowShelfGainKnob.getX(), lowShelfGainKnob.getY() - 15,
                             lowShelfGainKnob.getWidth(), 20);

    lowcutFreqKnob.setBounds(computeXcoord(1, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    lowcutFreqKnob.label.setBounds(lowcutFreqKnob.getX(), lowcutFreqKnob.getY() - 15,
                             lowcutFreqKnob.getWidth(), 20);

    lowMidFreqKnob.setBounds(computeXcoord(2, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    lowMidFreqKnob.label.setBounds(lowMidFreqKnob.getX(), lowMidFreqKnob.getY() - 15,
                             lowMidFreqKnob.getWidth(), 20);
    lowMidGainKnob.setBounds(computeXcoord(2, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    lowMidGainKnob.label.setBounds(lowMidGainKnob.getX(), lowMidGainKnob.getY() - 15,
                             lowMidGainKnob.getWidth(), 20);
    lowMidQKnob.setBounds(computeXcoord(2, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    lowMidQKnob.label.setBounds(lowMidQKnob.getX(), lowMidQKnob.getY() - 15,
                             lowMidQKnob.getWidth(), 20);

    midFreqKnob.setBounds(computeXcoord(3, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    midFreqKnob.label.setBounds(midFreqKnob.getX(), midFreqKnob.getY() - 15,
                             midFreqKnob.getWidth(), 20);
    midGainKnob.setBounds(computeXcoord(3, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    midGainKnob.label.setBounds(midGainKnob.getX(), midGainKnob.getY() - 15,
                             midGainKnob.getWidth(), 20);
    midQKnob.setBounds(computeXcoord(3, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    midQKnob.label.setBounds(midQKnob.getX(), midQKnob.getY() - 15,
                             midQKnob.getWidth(), 20);

    highFreqKnob.setBounds(computeXcoord(4, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    highFreqKnob.label.setBounds(highFreqKnob.getX(), highFreqKnob.getY() - 15,
                             highFreqKnob.getWidth(), 20);
    highGainKnob.setBounds(computeXcoord(4, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    highGainKnob.label.setBounds(highGainKnob.getX(), highGainKnob.getY() - 15,
                             highGainKnob.getWidth(), 20);
    highQKnob.setBounds(computeXcoord(4, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    highQKnob.label.setBounds(highQKnob.getX(), highQKnob.getY() - 15,
                             highQKnob.getWidth(), 20);

    highShelfFreqKnob.setBounds(computeXcoord(5, width), computeYcoord(0, height), eqKnobSize, eqKnobSize);
    highShelfFreqKnob.label.setBounds(highShelfFreqKnob.getX(), highShelfFreqKnob.getY() - 15,
                             highShelfFreqKnob.getWidth(), 20);
    highShelfGainKnob.setBounds(computeXcoord(5, width), computeYcoord(1, height), eqKnobSize, eqKnobSize);
    highShelfGainKnob.label.setBounds(highShelfGainKnob.getX(), highShelfGainKnob.getY() - 15,
                             highShelfGainKnob.getWidth(), 20);

    highcutFreqKnob.setBounds(computeXcoord(5, width), computeYcoord(2, height), eqKnobSize, eqKnobSize);
    highcutFreqKnob.label.setBounds(highcutFreqKnob.getX(), highcutFreqKnob.getY() - 15,
                             highcutFreqKnob.getWidth(), 20);

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

    juce::Colour colour = findColour (juce::ResizableWindow::backgroundColourId);
    tabs.addTab("Input", colour, &gateBoostPage, true);
    tabs.addTab("Amplifier", colour, &ampPage, true);
    tabs.addTab("Gain Stages", colour, &gainStagesPage, true);
    tabs.addTab("EQ", colour, &eqPage, true);
    tabs.addTab("IR Loader", colour, &irLoaderPage, true);

    addAndMakeVisible(tabs);
    addAndMakeVisible(volumePanel);

    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::darkred);

    setSize(windoWidth, windowHeight);
    setResizable(false, false);
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
