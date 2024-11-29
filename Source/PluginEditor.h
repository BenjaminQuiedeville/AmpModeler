/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "PluginProcessor.h"

const juce::String defaultIRText = "Default IR, made by JuanPabloZed with IR Maker \nhttps://github.com/JuanPabloZed/IR_Maker_Cpp";

const int knobSize = 100;
const int horizontalMargin = 25;
const int verticalMargin = 30;
const int nRows = 3;
const int nCols = 6;

const int windoWidth = 1000;
const int windowHeight = 450;

using Apvts = juce::AudioProcessorValueTreeState;
using SliderAttachment = Apvts::SliderAttachment;
using ComboBoxAttachment = Apvts::ComboBoxAttachment;
using ButtonAttachment = Apvts::ButtonAttachment;

struct Knob : public juce::Slider {
    Knob(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix);

    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};


struct VSlider : public juce::Slider {
    VSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix);

    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};

struct HSlider : public juce::Slider {
    HSlider(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts, juce::String suffix);

    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
    juce::Identifier paramID;
};

struct ComboBox : public juce::ComboBox {
    ComboBox(juce::String labelID, juce::String name, juce::Component *comp, juce::String paramID, Apvts &apvts);

    juce::Label label;
    std::unique_ptr<ComboBoxAttachment> boxAttachment;
};


struct GateBoostPage : public juce::Component {
    GateBoostPage(Processor &p);
    void resized();

    Knob gateKnob;
    // Knob gateAttackKnob;
    // Knob gateReleaseKnob;
    // Knob gateReturnKnob;

    Knob boostAttackKnob;
    Knob boostFreqKnob;
    Knob boostTightKnob;

    Knob inputGainKnob;

    juce::ToggleButton gateToggle {"Activate Gate/Boost"};
    juce::ToggleButton preampToggle {"Activate Preamp"};
    juce::ToggleButton tonestackToggle {"Activate Tonestack"};
};


struct AmplifierPage : public juce::Component {
    AmplifierPage(Processor &p);
    void resized();

    Knob gainKnob;
    Knob inputFilterKnob;
    Knob bassEQKnob;
    Knob midEQKnob;
    Knob trebbleEQKnob;
    Knob preampVolumeKnob;

    Knob resonanceKnob;
    Knob presenceKnob;

    ComboBox ampChannelBox;
    ComboBox toneStackModelBox;
    ComboBox channelConfigBox;
};


struct GainStagesPage : public juce::Component {
    GainStagesPage(Processor &p);
    void resized();

    juce::TextButton resetButton {"Reset"};

    HSlider stage0LPSlider;
    HSlider stage0BypassSlider;
    HSlider stage0BiasSlider;

    HSlider stage1HPSlider;
    HSlider stage1LPSlider;
    HSlider stage1BypassSlider;
    HSlider stage1BiasSlider;

    HSlider stage2HPSlider;
    HSlider stage2LPSlider;
    HSlider stage2BypassSlider;
    HSlider stage2BiasSlider;

    HSlider stage3HPSlider;
    HSlider stage3LPSlider;
    HSlider stage3BypassSlider;
    HSlider stage3BiasSlider;

    HSlider stage4HPSlider;
    HSlider stage4LPSlider;
    HSlider stage4BypassSlider;
    HSlider stage4BiasSlider;
};


struct IRLoaderPage : public juce::Component {
    IRLoaderPage(Processor &audioProcessor);
    void resized();

    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::TextButton nextIRButton {"Next"};
    juce::TextButton prevIRButton {"Previous"};

    juce::TextButton defaultIRButton {"Load default IR"};

    juce::ToggleButton bypassToggle {"Activate IRloader"};
    std::unique_ptr<ButtonAttachment> bypassButtonAttachment;
};


struct EQPage : public juce::Component {
    EQPage(Processor &audioProcessor);
    void resized();

    juce::TextButton resetButton {"Reset Gains"};

    Knob lowcutFreqKnob;

    Knob lowShelfFreqKnob;
    Knob lowShelfGainKnob;

    Knob lowMidFreqKnob;
    Knob lowMidGainKnob;
    Knob lowMidQKnob;

    Knob midFreqKnob;
    Knob midGainKnob;
    Knob midQKnob;

    Knob highFreqKnob;
    Knob highGainKnob;
    Knob highQKnob;

    Knob highShelfFreqKnob;
    Knob highShelfGainKnob;

    Knob highcutFreqKnob;
};


struct MasterVolPanel : public juce::Component {

    MasterVolPanel(Processor &p) :
        volumeSlider("MASTER_VOLUME_SLIDER_LABEL", "Master Vol", this, ParamIDs[MASTER_VOLUME].toString(), p.apvts, " dB")
    {
    }

    void paint(juce::Graphics& g) override {
        g.fillAll(juce::Colours::darkgrey);
    }


    void resized() {

        volumeSlider.setBounds(0, 50, getWidth(), 300);
        volumeSlider.label.setBounds(volumeSlider.getX(),volumeSlider.getY() - 15,
                                    volumeSlider.getWidth(), 20);
    }

    VSlider volumeSlider;
};


struct Editor : public juce::AudioProcessorEditor
{
    Editor (Processor&);

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    juce::TabbedComponent tabs {juce::TabbedButtonBar::TabsAtTop};
    juce::Component statusBar;

    GateBoostPage gateBoostPage;
    AmplifierPage ampPage;
    GainStagesPage gainStagesPage;
    EQPage eqPage;
    IRLoaderPage irLoaderPage;

    MasterVolPanel volumePanel;

    Processor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);
    void createComboBox(ComboBox *comboBox, const juce::String& paramID);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
