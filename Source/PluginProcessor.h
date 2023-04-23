#pragma once

#include <JuceHeader.h>
#include "types.h"

#include "ThreeBandEQ.h"
#include "IRLoader.h"

/*
RESTANT : 
    input processing 
    gate
    pre boost
    amp {input disto EQ}
    Compression 
    re EQ
*/


class AmpModelerAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
	//==============================================================================
	AmpModelerAudioProcessor();
	~AmpModelerAudioProcessor() override;

	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

	#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
	#endif

	void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const juce::String getProgramName (int index) override;
	void changeProgramName (int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation (juce::MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;

	juce::AudioProcessorValueTreeState apvts;

    ThreeBandEQ postEQ;
    IRLoader irLoader;

private:

	juce::dsp::ProcessSpec spec;

	juce::dsp::Gain<sample_t> masterVolume;


    void safetyClip(AudioBlock &audioBlock);


	juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessor)
};
