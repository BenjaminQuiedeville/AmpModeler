/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "common.h"
#include "SmoothParam.h"

#include "NoiseGate.h"
#include "Preamp.h"
#include "Tonestack.h"
#include "IRLoader.h"
#include "FirstOrderShelf.h"

#include "Biquad.h"

#include <vector>

#ifndef  JucePlugin_Name
    #define JucePlugin_Name "AmpModeler"
#endif

#define BOOST_BITE_Q     0.25

#define RESONANCE_FREQUENCY 250.0
#define PRESENCE_FREQUENCY  500.0


/*
Stereo 
    MONO
    FAKE STEREO
    STEREO
    
pour tout le process :
    faire des fonctions indépendantes pour les deux coté 
    si on passe un nullptr dans les fontions pour le coté droit, on early return et on process pas à droite
    

RESTANT : 

général 
    presets

tonestack
    composants custom
    
preamp 
    modèle de lampe

    filtres custom ?

irloader 

    wavread
        ignorer les chunks inutils

    double IR mixable
    alignement de phase

    algorithme partitionné
*/

enum Params {
    GATE_THRESH,
    GATE_ATTACK,
    GATE_RELEASE,
    GATE_RETURN,
    BITE,
    BITE_FREQ,
    TIGHT,
    PREAMP_GAIN,
    CHANNEL,
    INPUT_FILTER,
    TONESTACK_MODEL,
    TONESTACK_BASS,
    TONESTACK_MIDDLE,
    TONESTACK_TREBBLE,
    PREAMP_VOLUME,
    RESONANCE,
    PRESENCE,
    MASTER_VOLUME,
    CHANNEL_CONFIG,
    N_PARAMS
};

static std::vector<juce::Identifier> ParamIDs = {
    "GATE_THRESH",
    "GATE_ATTACK",
    "GATE_RELEASE",
    "GATE_RETURN",
    "BITE",
    "BITE_FREQ",
    "TIGHT",
    "PREAMP_GAIN",
    "INPUT_FILTER",
    "CHANNEL",
    "TONE_MODEL",
    "TONE_BASS",
    "TONE_MID",
    "TONE_TREB",
    "PREAMP_VOLUME",
    "RESONANCE",
    "PRESENCE",
    "MASTER_VOLUME",
    "CHANNEL_CONFIG"
};

enum ChannelConfig {
    Mono, 
    FakeStereo, 
    Stereo
};

static juce::Identifier irPathTree("IR_PATHS");
static juce::Identifier irPath1 ("IR1");
static juce::Identifier irPath2 ("IR2");

struct Processor  : public juce::AudioProcessor, 
                                   juce::AudioProcessorValueTreeState::Listener
                                   #if JucePlugin_Enable_ARA
                                   , public juce::AudioProcessorARAExtension
                                   #endif
{
	//==============================================================================
	Processor();
	~Processor() override;

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

    juce::ValueTree valueTree;
	juce::AudioProcessorValueTreeState apvts;

    Biquad inputNoiseFilter {BIQUAD_LOWPASS};

    OnepoleFilter tightFilter;
    Biquad biteFilter {BIQUAD_PEAK};

    NoiseGate *noiseGate;
    Preamp *preamp;
    Tonestack *toneStack;
    IRLoader *irLoader;

    FirstOrderShelfFilter resonanceFilter {lowshelf};
    FirstOrderShelfFilter presenceFilter {highshelf};

	SmoothParamLinear masterVolume;

    double samplerate = 44100.0;

    Sample *sideChainBuffer = nullptr;
    
    u8 channelConfig = Mono;

    void initParameters();
    void parameterChanged(const juce::String &parameterID, float newValue) override;

	juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
