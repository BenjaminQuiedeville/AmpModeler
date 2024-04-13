/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

struct Editor;

struct Knob : public juce::Slider {

    Knob(juce::String labelID, juce::String name, Editor* editor);
    
    void init(juce::String paramID, Editor *editor);
    
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};


struct ComboBox : public juce::ComboBox {
    
    ComboBox(juce::String labelID, juce::String name, Editor* editor); 

    void init(juce::String paramID, Editor *editor);

    juce::Label label;
    std::unique_ptr<ComboBoxAttachment> boxAttachment;
};


struct Editor : public juce::AudioProcessorEditor
{
    Editor (Processor&);
    ~Editor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    Knob gateKnob;
    Knob boostAttackKnob;
    Knob boostFreqKnob;
    Knob boostTightKnob;
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

    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::ToggleButton testOscToggle {"Toggle Test Sound"};
    juce::ToggleButton testOscNoiseToggle {"Noise toggle"};
    juce::ToggleButton irLoaderBypassToggle {"Bypass IRloader"};

    juce::TabbedComponent tabs {juce::TabbedButtonBar::Orientation::TabsAtTop};

    Processor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);
    void createComboBox(ComboBox *comboBox, const juce::String& paramID);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Editor)
};
