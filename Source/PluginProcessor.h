#pragma once

#include "JuceHeader.h"
#include "common.h"
#include "SmoothParam.h"

#include "NoiseGate.h"
#include "Boost.h"
#include "Preamp.h"
#include "Tonestack.h"
#include "IRLoader.h"

#include "Biquad.h"

#include <vector>

#define RESONANCE_FREQUENCY 250.0
#define RESONANCE_Q         0.7
#define PRESENCE_FREQUENCY  500.0
#define PRESENCE_Q          0.4


/*
RESTANT : 
    gate
*/

enum Params {
    GATE_THRESH,
    BITE,
    TIGHT,
    PRE_BOOST,
    PREAMP_GAIN,
    INPUT_FILTER,
    TONESTACK_BASS,
    TONESTACK_MIDDLE,
    TONESTACK_TREBBLE,
    PREAMP_VOLUME,
    RESONANCE,
    PRESENCE,
    MASTER_VOLUME,
    N_PARAMS
};

const std::vector<juce::String> ParamsID = {
    "GATE_THRESH",
    "BITE",
    "TIGHT",
    "PRE_BOOST",
    "PREAMP_GAIN",
    "INPUT_FILTER",
    "TONESTACK_BASS",
    "TONESTACK_MIDDLE",
    "TONESTACK_TREBBLE",
    "PREAMP_VOLUME",
    "RESONANCE",
    "PRESENCE",
    "MASTER_VOLUME"
};

class AmpModelerAudioProcessor  : public juce::AudioProcessor, 
                                juce::AudioProcessorValueTreeState::Listener
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

	juce::AudioProcessorValueTreeState *apvts;

    NoiseGate *noiseGate;
    Boost *preBoost;
    PreampDistorsion *preamp;
    Tonestack *toneStack;
    IRLoader *irLoader;

    Biquad *resonanceFilter;
    Biquad *presenceFilter;

	SmoothParamLinear *masterVolume;

    double samplerate = 44100.0;

    sample_t *intputSignalCopy = nullptr;

private:


    void initParameters();
    void parameterChanged(const juce::String &parameterID, float newValue) override;

	juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessor)
};
