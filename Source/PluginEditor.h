#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"


class AmpModelerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
	AmpModelerAudioProcessorEditor (AmpModelerAudioProcessor&);
	~AmpModelerAudioProcessorEditor() override;

	//==============================================================================
	void paint (juce::Graphics&) override;
	void resized() override;

private:

    juce::Slider gateSlider;
    juce::Label gateSliderLabel {"GATE_SLIDER_LABEL", "Gate Thresh"};
    std::unique_ptr<SliderAttachment> gateSliderAttachment; 

    juce::Slider gainSlider;
    juce::Label gainSliderLabel {"GAIN_SLIDER_LABEL", "Pre Gain"};
    std::unique_ptr<SliderAttachment> gainSliderAttachment;

    juce::Slider bassEQSlider;
    juce::Label bassEQSliderLabel {"BASS_EQ_SLIDER_LABEL", "Bass"};
    std::unique_ptr<SliderAttachment> bassEQSliderAttachment;

    juce::Slider midEQSlider;
    juce::Label midEQSliderLabel {"MIDDLE_EQ_SLIDER_LABEL", "Mid"};
    std::unique_ptr<SliderAttachment> midEQSliderAttachment;

    juce::Slider trebbleEQSlider;
    juce::Label trebbleEQSliderLabel {"TREBBLE_EQ_SLIDER_LABEL", "Trebble"};
    std::unique_ptr<SliderAttachment> trebbleEQSliderAttachment;

    juce::Slider preampVolumeSlider;
    juce::Label preampVolumeSliderLabel {"PREMP_VOLUME_SLIDER_LABEL", "Post Gain"};
    std::unique_ptr<SliderAttachment> preampVolumeSliderAttachment;


	juce::Slider volumeSlider;
    juce::Label volumeSliderLabel {"MASTER_VOLUME_SLIDER_LABEL", "Output Level"};
    std::unique_ptr<SliderAttachment> volumeSliderAttachment;


    juce::TextButton irLoadButton {"Load IR"};


    AmpModelerAudioProcessor& audioProcessor;

    void createKnob(juce::Slider &slider, 
					juce::Label &label, 
					std::unique_ptr<SliderAttachment> &sliderAttchmnt, 
					const std::string paramID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessorEditor)
};
