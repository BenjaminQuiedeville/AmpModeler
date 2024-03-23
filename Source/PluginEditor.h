/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include "PluginProcessor.h"

using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;


struct Knob {

    Knob(juce::String labelID, juce::String name) : label{labelID, name} 
    {
    }

    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;
};


struct ComboBox {
    
    ComboBox(juce::String labelID, juce::String name) : label {labelID, name}
    {
    }

    juce::ComboBox box;
    juce::Label label;
    std::unique_ptr<ComboBoxAttachment> boxAttachment;
};


class AmpModelerAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    AmpModelerAudioProcessorEditor (AmpModelerAudioProcessor&);
    ~AmpModelerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    std::unique_ptr<Knob> gateKnob;
    std::unique_ptr<Knob> boostTopKnob;
    std::unique_ptr<Knob> boostTightKnob;
    std::unique_ptr<Knob> gainKnob;
    std::unique_ptr<Knob> inputFilterKnob;
    std::unique_ptr<Knob> bassEQKnob;
    std::unique_ptr<Knob> midEQKnob;
    std::unique_ptr<Knob> trebbleEQKnob;
    std::unique_ptr<Knob> preampVolumeKnob;

    std::unique_ptr<Knob> resonanceKnob;
    std::unique_ptr<Knob> presenceKnob;

    std::unique_ptr<Knob> volumeKnob;

    std::unique_ptr<ComboBox> ampChannelBox;
    std::unique_ptr<ComboBox> toneStackModelBox;

    juce::TextButton irLoadButton {"Load IR"};
    juce::Label irNameLabel {"IR_NAME_LABEL", "Default IR"};

    juce::ToggleButton testOscToggle {"Toggle Test Sound"};
    juce::ToggleButton testOscNoiseToggle {"Noise toggle"};
    juce::ToggleButton irLoaderBypassToggle {"Bypass IRloader"};

    AmpModelerAudioProcessor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);
    void createComboBox(ComboBox *comboBox, const juce::String& paramID);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessorEditor)
};
