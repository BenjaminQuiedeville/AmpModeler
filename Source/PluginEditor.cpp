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
    gateKnob("GATE_KNOB_LABEL", "Gate Thresh", this),
    // gateAttackKnob("GATE_Attack_LABEL", "Gate Attack", this),
    // gateReleaseKnob("GATE_RELEASE_LABEL", "Gate Release", this), 
    // gateReturnKnob("GATE_RETURN_LABEL", "Gate Hysteresis", this),

    boostAttackKnob("BOOST_ATTACK_KNOB_LABEL" , "Boost Top", this),
    boostFreqKnob("BOOST_FREQ_KNOB_LABEL", "Boost Freq", this),
    boostTightKnob("BOOST_TIGHT_KNOB_LABEL", "Boost Tight", this),
    inputGainKnob("INPUT_GAIN_KNOB_LABEL", "Input Gain", this)

{
    gateKnob.init(ParamIDs[GATE_THRESH].toString(), p.apvts);
    gateKnob.setTextValueSuffix(" dB");

    // gateAttackKnob.init(ParamIDs[GATE_ATTACK].toString(), p.apvts);
    // gateAttackKnob.setTextValueSuffix(" ms");
    
    // gateReleaseKnob.init(ParamIDs[GATE_RELEASE].toString(), p.apvts);
    // gateReleaseKnob.setTextValueSuffix(" ms");
    
    // gateReturnKnob.init(ParamIDs[GATE_RETURN].toString(), p.apvts);
    // gateReturnKnob.setTextValueSuffix(" dB");

    inputGainKnob.init(ParamIDs[INPUT_GAIN].toString(), p.apvts);
    inputGainKnob.setTextValueSuffix(" dB");

    boostAttackKnob.init(ParamIDs[SCREAMER_AMOUNT].toString(), p.apvts);
    boostAttackKnob.setTextValueSuffix(" dB");
    
    boostFreqKnob.init(ParamIDs[SCREAMER_FREQ].toString(), p.apvts);
    boostFreqKnob.setTextValueSuffix(" Hz");
    
    boostTightKnob.init(ParamIDs[TIGHT].toString(), p.apvts);
    boostTightKnob.setTextValueSuffix(" Hz");
    
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
    inputGainKnob.label.setBounds(inputGainKnob.getX(), 
                              inputGainKnob.getY() - 15, 
                              inputGainKnob.getWidth(), 
                              20);

    gateKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    gateKnob.label.setBounds(gateKnob.getX(), 
                              gateKnob.getY() - 15, 
                              gateKnob.getWidth(), 
                              20);


    // gateAttackKnob.setBounds(computeXcoord(1, width), computeYcoord(0, height), knobSize, knobSize);
    // gateAttackKnob.label.setBounds(gateAttackKnob.getX(),
    //                                gateAttackKnob.getY() - 15, 
    //                                gateAttackKnob.getWidth(), 
    //                                20);
                                    
    // gateReleaseKnob.setBounds(computeXcoord(1, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReleaseKnob.label.setBounds(gateReleaseKnob.getX(),
    //                                 gateReleaseKnob.getY() - 15, 
    //                                 gateReleaseKnob.getWidth(), 
    //                                 20);
                                    
    // gateReturnKnob.setBounds(computeXcoord(0, width), computeYcoord(1, height), knobSize, knobSize);
    // gateReturnKnob.label.setBounds(gateReturnKnob.getX(),
    //                                gateReturnKnob.getY() - 15, 
    //                                gateReturnKnob.getWidth(), 
    //                                20);
                                    


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

    gateToggle.setBounds(computeXcoord(4, width), computeYcoord(0, height), 200, 30);
    preampToggle.setBounds(gateToggle.getBounds() + juce::Point(0, gateToggle.getHeight() + 20));
    tonestackToggle.setBounds(preampToggle.getBounds() + juce::Point(0, preampToggle.getHeight() + 20));
}


AmplifierPage::AmplifierPage(Processor &p) :
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
    gainKnob.init(ParamIDs[PREAMP_GAIN].toString(), p.apvts);
    gainKnob.setNumDecimalPlacesToDisplay(2);
    inputFilterKnob.init(ParamIDs[INPUT_FILTER].toString(), p.apvts);
    inputFilterKnob.setTextValueSuffix(" Hz");
    
    bassEQKnob.init(ParamIDs[TONESTACK_BASS].toString(), p.apvts);
    
    midEQKnob.init(ParamIDs[TONESTACK_MIDDLE].toString(), p.apvts);
    
    trebbleEQKnob.init(ParamIDs[TONESTACK_TREBBLE].toString(), p.apvts);
    
    preampVolumeKnob.init(ParamIDs[PREAMP_VOLUME].toString(), p.apvts);
    preampVolumeKnob.setTextValueSuffix(" dB");
 
    resonanceKnob.init(ParamIDs[RESONANCE].toString(), p.apvts);
    presenceKnob.init(ParamIDs[PRESENCE].toString(), p.apvts);
 
    ampChannelBox.init(ParamIDs[CHANNEL].toString(), p.apvts);
    toneStackModelBox.init(ParamIDs[TONESTACK_MODEL].toString(), p.apvts);
    channelConfigBox.init(ParamIDs[CHANNEL_CONFIG].toString(), p.apvts);

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

GainStagesPage::GainStagesPage(Processor &p) : 
    stage0LPSlider("STAGE0_LP_SLIDER_LABEL", "Stage 0 output filter", this),
    stage0BypassSlider("STAGE0_BYPASS_SLIDER_LABEL", "Stage 0 cathode bypass", this),
    stage0BiasSlider("STAGE0_BIAS_SLIDER_LABEL", "Stage 0 tube bias", this),

    stage1HPSlider("STAGE1_HP_SLIDER_LABEL", "Stage 1 coupling filter", this),
    stage1LPSlider("STAGE1_LP_SLIDER_LABEL", "Stage 1 output filter", this),
    stage1BypassSlider("STAGE1_BYPASS_SLIDER_LABEL", "Stage 1 cathode bypass", this),
    stage1BiasSlider("STAGE1_BIAS_SLIDER_LABEL", "Stage 1 tube bias", this),

    stage2HPSlider("STAGE2_HP_SLIDER_LABEL", "Stage 2 coupling filter", this),
    stage2LPSlider("STAGE2_LP_SLIDER_LABEL", "Stage 2 output filter", this),
    stage2BypassSlider("STAGE2_BYPASS_SLIDER_LABEL", "Stage 2 cathode bypass", this),
    stage2BiasSlider("STAGE2_BIAS_SLIDER_LABEL", "Stage 2 tube bias", this),
    
    stage3HPSlider("STAGE3_HP_SLIDER_LABEL", "Stage 3 coupling filter", this),
    stage3LPSlider("STAGE3_LP_SLIDER_LABEL", "Stage 3 output filter", this),
    stage3BypassSlider("STAGE3_BYPASS_SLIDER_LABEL", "Stage 3 cathode bypass", this),
    stage3BiasSlider("STAGE3_BIAS_SLIDER_LABEL", "Stage 3 tube bias", this),

    stage4HPSlider("STAGE4_HP_SLIDER_LABEL", "Stage 4 coupling filter", this),
    stage4LPSlider("STAGE4_LP_SLIDER_LABEL", "Stage 4 output filter", this),
    stage4BypassSlider("STAGE4_BYPASS_SLIDER_LABEL", "Stage 4 cathode bypass", this),
    stage4BiasSlider("STAGE4_BIAS_SLIDER_LABEL", "Stage 4 tube bias", this)
{
    stage0LPSlider.init(ParamIDs[STAGE0_LP].toString(), p.apvts);
    stage0LPSlider.setTextValueSuffix(" Hz");
    stage0BypassSlider.init(ParamIDs[STAGE0_BYPASS].toString(), p.apvts);
    stage0BypassSlider.setTextValueSuffix(" dB");
    stage0BiasSlider.init(ParamIDs[STAGE0_BIAS].toString(), p.apvts);

    stage1HPSlider.init(ParamIDs[STAGE1_HP].toString(), p.apvts);
    stage1HPSlider.setTextValueSuffix(" Hz");
    stage1LPSlider.init(ParamIDs[STAGE1_LP].toString(), p.apvts);
    stage1LPSlider.setTextValueSuffix(" Hz");
    stage1BypassSlider.init(ParamIDs[STAGE1_BYPASS].toString(), p.apvts);
    stage1BypassSlider.setTextValueSuffix(" dB");
    stage1BiasSlider.init(ParamIDs[STAGE1_BIAS].toString(), p.apvts);

    stage2HPSlider.init(ParamIDs[STAGE2_HP].toString(), p.apvts);
    stage2HPSlider.setTextValueSuffix(" Hz");
    stage2LPSlider.init(ParamIDs[STAGE2_LP].toString(), p.apvts);
    stage2LPSlider.setTextValueSuffix(" Hz");
    stage2BypassSlider.init(ParamIDs[STAGE2_BYPASS].toString(), p.apvts);
    stage2BypassSlider.setTextValueSuffix(" dB");
    stage2BiasSlider.init(ParamIDs[STAGE2_BIAS].toString(), p.apvts);
    
    stage3HPSlider.init(ParamIDs[STAGE3_HP].toString(), p.apvts);
    stage3HPSlider.setTextValueSuffix(" Hz");
    stage3LPSlider.init(ParamIDs[STAGE3_LP].toString(), p.apvts);
    stage3LPSlider.setTextValueSuffix(" Hz");
    stage3BypassSlider.init(ParamIDs[STAGE3_BYPASS].toString(), p.apvts);
    stage3BypassSlider.setTextValueSuffix(" dB");
    stage3BiasSlider.init(ParamIDs[STAGE3_BIAS].toString(), p.apvts);

    stage4HPSlider.init(ParamIDs[STAGE4_HP].toString(), p.apvts);
    stage4HPSlider.setTextValueSuffix(" Hz");
    stage4LPSlider.init(ParamIDs[STAGE4_LP].toString(), p.apvts);
    stage4LPSlider.setTextValueSuffix(" Hz");
    stage4BypassSlider.init(ParamIDs[STAGE4_BYPASS].toString(), p.apvts);
    stage4BypassSlider.setTextValueSuffix(" dB");
    stage4BiasSlider.init(ParamIDs[STAGE4_BIAS].toString(), p.apvts);

}

void GainStagesPage::resized() {

    static const int verticalSpacing = 30;
    static const int horizontalSpacing = 20;

    stage0LPSlider.setBounds(horizontalMargin, verticalMargin + 50 + verticalSpacing, 150, 50);
    stage0LPSlider.label.setBounds(stage0LPSlider.getX(),
                                    stage0LPSlider.getY() - 15,
                                    stage0LPSlider.getWidth(),
                                    20);

    stage0BypassSlider.setBounds(stage0LPSlider.getBounds() + juce::Point(0, stage0LPSlider.getHeight() + verticalSpacing));
    stage0BypassSlider.label.setBounds(stage0BypassSlider.getX(),
                                    stage0BypassSlider.getY() - 15,
                                    stage0BypassSlider.getWidth(),
                                    20);
                                    
    stage0BiasSlider.setBounds(stage0BypassSlider.getBounds() + juce::Point(0, stage0BypassSlider.getHeight() + verticalSpacing));
    stage0BiasSlider.label.setBounds(stage0BiasSlider.getX(),
                                    stage0BiasSlider.getY() - 15,
                                    stage0BiasSlider.getWidth(),
                                    20);
                                    

    stage1HPSlider.setBounds(stage0LPSlider.getRight() + horizontalSpacing, verticalMargin, stage0LPSlider.getWidth(), stage0LPSlider.getHeight());
    stage1HPSlider.label.setBounds(stage1HPSlider.getX(),
                                    stage1HPSlider.getY() - 15,
                                    stage1HPSlider.getWidth(),
                                    20);
                                    
    stage1LPSlider.setBounds(stage1HPSlider.getBounds() + juce::Point(0, stage1HPSlider.getHeight() + verticalSpacing));
    stage1LPSlider.label.setBounds(stage1LPSlider.getX(),
                                    stage1LPSlider.getY() - 15,
                                    stage1LPSlider.getWidth(),
                                    20);
                                    
    stage1BypassSlider.setBounds(stage1LPSlider.getBounds() + juce::Point(0, stage1LPSlider.getHeight() + verticalSpacing));
    stage1BypassSlider.label.setBounds(stage1BypassSlider.getX(),
                                    stage1BypassSlider.getY() - 15,
                                    stage1BypassSlider.getWidth(),
                                    20);
                                    
    stage1BiasSlider.setBounds(stage1BypassSlider.getBounds() + juce::Point(0, stage1BypassSlider.getHeight() + verticalSpacing));
    stage1BiasSlider.label.setBounds(stage1BiasSlider.getX(),
                                    stage1BiasSlider.getY() - 15,
                                    stage1BiasSlider.getWidth(),
                                    20);
                                    

    stage2HPSlider.setBounds(stage1LPSlider.getRight() + horizontalSpacing, verticalMargin, stage1LPSlider.getWidth(), stage1LPSlider.getHeight());
    stage2HPSlider.label.setBounds(stage2HPSlider.getX(),
                                    stage2HPSlider.getY() - 15,
                                    stage2HPSlider.getWidth(),
                                    20);
                                    
    stage2LPSlider.setBounds(stage2HPSlider.getBounds() + juce::Point(0, stage2HPSlider.getHeight() + verticalSpacing));
    stage2LPSlider.label.setBounds(stage2LPSlider.getX(),
                                    stage2LPSlider.getY() - 15,
                                    stage2LPSlider.getWidth(),
                                    20);
                                    
    stage2BypassSlider.setBounds(stage2LPSlider.getBounds() + juce::Point(0, stage2LPSlider.getHeight() + verticalSpacing));
    stage2BypassSlider.label.setBounds(stage2BypassSlider.getX(),
                                    stage2BypassSlider.getY() - 15,
                                    stage2BypassSlider.getWidth(),
                                    20);
                                    
    stage2BiasSlider.setBounds(stage2BypassSlider.getBounds() + juce::Point(0, stage2BypassSlider.getHeight() + verticalSpacing));
    stage2BiasSlider.label.setBounds(stage2BiasSlider.getX(),
                                    stage2BiasSlider.getY() - 15,
                                    stage2BiasSlider.getWidth(),
                                    20);
                                    
    

    stage3HPSlider.setBounds(stage2LPSlider.getRight() + horizontalSpacing, verticalMargin, stage2LPSlider.getWidth(), stage2LPSlider.getHeight());
    stage3HPSlider.label.setBounds(stage3HPSlider.getX(),
                                    stage3HPSlider.getY() - 15,
                                    stage3HPSlider.getWidth(),
                                    20);

    stage3LPSlider.setBounds(stage3HPSlider.getBounds() + juce::Point(0, stage3HPSlider.getHeight() + verticalSpacing));
    stage3LPSlider.label.setBounds(stage3LPSlider.getX(),
                                    stage3LPSlider.getY() - 15,
                                    stage3LPSlider.getWidth(),
                                    20);
                                    
    stage3BypassSlider.setBounds(stage3LPSlider.getBounds() + juce::Point(0, stage3LPSlider.getHeight() + verticalSpacing));
    stage3BypassSlider.label.setBounds(stage3BypassSlider.getX(),
                                    stage3BypassSlider.getY() - 15,
                                    stage3BypassSlider.getWidth(),
                                    20);
                                    
    stage3BiasSlider.setBounds(stage3BypassSlider.getBounds() + juce::Point(0, stage3BypassSlider.getHeight() + verticalSpacing));
    stage3BiasSlider.label.setBounds(stage3BiasSlider.getX(),
                                    stage3BiasSlider.getY() - 15,
                                    stage3BiasSlider.getWidth(),
                                    20);
                                    

    stage4HPSlider.setBounds(stage3LPSlider.getRight() + horizontalSpacing, verticalMargin, stage3LPSlider.getWidth(), stage3LPSlider.getHeight());
    stage4HPSlider.label.setBounds(stage4HPSlider.getX(),
                                    stage4HPSlider.getY() - 15,
                                    stage4HPSlider.getWidth(),
                                    20);

    stage4LPSlider.setBounds(stage4HPSlider.getBounds() + juce::Point(0, stage4HPSlider.getHeight() + verticalSpacing));
    stage4LPSlider.label.setBounds(stage4LPSlider.getX(),
                                    stage4LPSlider.getY() - 15,
                                    stage4LPSlider.getWidth(),
                                    20);
                                    
    stage4BypassSlider.setBounds(stage4LPSlider.getBounds() + juce::Point(0, stage4LPSlider.getHeight() + verticalSpacing));
    stage4BypassSlider.label.setBounds(stage4BypassSlider.getX(),
                                    stage4BypassSlider.getY() - 15,
                                    stage4BypassSlider.getWidth(),
                                    20);
                                    
    stage4BiasSlider.setBounds(stage4BypassSlider.getBounds() + juce::Point(0, stage4BypassSlider.getHeight() + verticalSpacing));
    stage4BiasSlider.label.setBounds(stage4BiasSlider.getX(),
                                    stage4BiasSlider.getY() - 15,
                                    stage4BiasSlider.getWidth(),
                                    20);
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


//==============================================================================
Editor::Editor (Processor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),

    gateBoostPage(p),
    ampPage(p),
    gainStagesPage(p),
    irLoaderPage(p), 
    volumePanel(p)
{

    juce::Colour colour = findColour (juce::ResizableWindow::backgroundColourId);
    tabs.addTab("Input", colour, &gateBoostPage, true);
    tabs.addTab("Amplifier", colour, &ampPage, true);
    tabs.addTab("Gain Stages", colour, &gainStagesPage, true);
    tabs.addTab("IR Loader", colour, &irLoaderPage, true);
    
    addAndMakeVisible(tabs);
    addAndMakeVisible(volumePanel);

    setSize(1000, 400);
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
    irLoaderPage.setBounds(bounds);
    
    tabs.setBounds(bounds);
    

    gateBoostPage.resized();
    ampPage.resized();
    gainStagesPage.resized();
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


VSlider::VSlider(juce::String labelID, juce::String name, juce::Component *comp) 
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

void VSlider::init(juce::String paramID, Apvts &apvts) {
    sliderAttachment = std::make_unique<SliderAttachment>(
        apvts, paramID, *this
    );
}

HSlider::HSlider(juce::String labelID, juce::String name, juce::Component *comp) 
: label{labelID, name} 
{    
    setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 75, 25);
    comp->addAndMakeVisible(*this);

    label.setColour(juce::Label::ColourIds::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(15.0f);
    comp->addAndMakeVisible(label);
}

void HSlider::init(juce::String paramID, Apvts &apvts) {
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
