#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


struct Knob {

    Knob(juce::String labelID, juce::String name) : label{labelID, name} {
    }

    juce::Slider slider;
    juce::Label label;
    std::unique_ptr<SliderAttachment> sliderAttachment;

};

class AmpModelerAudioProcessorEditor  : public juce::AudioProcessorEditor
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

    // juce::Slider gateSlider;
    // juce::Label gateSliderLabel {"GATE_SLIDER_LABEL", "Gate Thresh"};
    // std::unique_ptr<SliderAttachment> gateSliderAttachment; 

    // juce::Slider gainSlider;
    // juce::Label gainSliderLabel {"GAIN_SLIDER_LABEL", "Pre Gain"};
    // std::unique_ptr<SliderAttachment> gainSliderAttachment;

    // juce::Slider inputFilterSlider;
    // juce::Label inputFilterSliderLabel {"INPUT_FILTER_LABEL", "Input Filter"};
    // std::unique_ptr<SliderAttachment> inputFilterSliderAttachment;

    // juce::Slider bassEQSlider;
    // juce::Label bassEQSliderLabel {"BASS_EQ_SLIDER_LABEL", "Bass"};
    // std::unique_ptr<SliderAttachment> bassEQSliderAttachment;

    // juce::Slider midEQSlider;
    // juce::Label midEQSliderLabel {"MIDDLE_EQ_SLIDER_LABEL", "Mid"};
    // std::unique_ptr<SliderAttachment> midEQSliderAttachment;

    // juce::Slider trebbleEQSlider;
    // juce::Label trebbleEQSliderLabel {"TREBBLE_EQ_SLIDER_LABEL", "Trebble"};
    // std::unique_ptr<SliderAttachment> trebbleEQSliderAttachment;

    // juce::Slider preampVolumeSlider;
    // juce::Label preampVolumeSliderLabel {"PREMP_VOLUME_SLIDER_LABEL", "Post Gain"};
    // std::unique_ptr<SliderAttachment> preampVolumeSliderAttachment;


	// juce::Slider volumeSlider;
    // juce::Label volumeSliderLabel {"MASTER_VOLUME_SLIDER_LABEL", "Output Level"};
    // std::unique_ptr<SliderAttachment> volumeSliderAttachment;


    juce::TextButton irLoadButton {"Load IR"};


    AmpModelerAudioProcessor& audioProcessor;

    // void createKnob(juce::Slider &slider, 
	// 				juce::Label &label, 
	// 				std::unique_ptr<SliderAttachment> &sliderAttchmnt, 
	// 				const juce::String& paramID);

    void createKnob(Knob *knob, const juce::String& paramID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessorEditor)
};
