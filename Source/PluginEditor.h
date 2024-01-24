#pragma once

#include "PluginProcessor.h"


struct Knob {

    Knob(juce::String labelID, juce::String name) : label{labelID, name} 
    {
    }

    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;

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


    juce::TextButton irLoadButton {"Load IR"};
    juce::ToggleButton testOscToggle {"Toggle Test Sound"};
    juce::ToggleButton testOscNoiseToggle {"Noise toggle"};


    AmpModelerAudioProcessor& audioProcessor;

    void createKnob(Knob *knob, const juce::String& paramID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessorEditor)
};
