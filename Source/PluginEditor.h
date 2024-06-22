/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

#include "PluginProcessor.h"

const juce::String defaultIRText = "Default IR, made by JuanPabloZed";

using Apvts = juce::AudioProcessorValueTreeState;
using SliderAttachment = Apvts::SliderAttachment;
using ComboBoxAttachment = Apvts::ComboBoxAttachment;


struct Editor;

struct Knob : public juce::Slider {

    Knob(juce::String labelID, juce::String name, juce::Component* comp);
    
    void init(juce::String paramID, Apvts &apvts);
    
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};


struct ComboBox : public juce::ComboBox {
    
    ComboBox(juce::String labelID, juce::String name, juce::Component* comp); 

    void init(juce::String paramID, Apvts &apvts);

    juce::Label label;
    std::unique_ptr<ComboBoxAttachment> boxAttachment;
};


struct GateBoostPage : public juce::Component {
    GateBoostPage(Apvts &apvts);
    void resized();
    
    Knob gateKnob;
    Knob boostAttackKnob;
    Knob boostFreqKnob;
    Knob boostTightKnob;

};


struct AmplifierPage : public juce::Component {
    AmplifierPage(Apvts &apvts);
    void resized();
    
    Knob gainKnob;
    Knob inputFilterKnob;
    Knob bassEQKnob;
    Knob midEQKnob;
    Knob trebbleEQKnob;
    Knob preampVolumeKnob;

    Knob resonanceKnob;
    Knob presenceKnob;

    Knob volumeKnob;

    ComboBox ampChannelBox;
    ComboBox toneStackModelBox;
    ComboBox channelConfigBox;    
};


struct GainStagesPage : public juce::Component {
    GainStagesPage() {
    
    }
    
    void resized() {
    
    }
};


struct IRLoaderPage : public juce::Component {
    IRLoaderPage(Processor &audioProcessor);
    void resized();
    
    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::TextButton irLoaderDefaultIRButton {"Load default IR"};
    juce::ToggleButton irLoaderBypassToggle {"Bypass IRloader"};

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
    IRLoaderPage irLoaderPage;


    Processor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);
    void createComboBox(ComboBox *comboBox, const juce::String& paramID);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
