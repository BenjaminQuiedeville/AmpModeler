#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "common.h"
#include "SmoothParam.h"

#include "NoiseGate.h"
#include "Boost.h"
#include "Preamp.h"
#include "Tonestack.h"
#include "IRLoader.h"

#include "Biquad.h"

#include <vector>
#include <random>

#ifndef  JucePlugin_Name
    #define JucePlugin_Name "AmpModeler"
#endif

#define RESONANCE_FREQUENCY 250.0
#define RESONANCE_Q         0.4
#define PRESENCE_FREQUENCY  500.0
#define PRESENCE_Q          0.4


/*
RESTANT : 
    gate 

    test sample player ? 
*/

struct TestOsc {

    void setFreq(double freq, double samplerate) {
        a = 2.0f * (float)sin(juce::MathConstants<double>::pi * freq/samplerate);
    }

    sample_t generateNextSample() {
        
        if (doNoise) {
            return gain * (std::rand() / RAND_MAX * 2.0f - 1.0f) ;
        }
            
        s[0] = s[0] - a*s[1];
        s[1] = s[1] + a*s[0];
        return s[0] * gain;
    }

    sample_t s[2] = {0.5f, 0.0f};
    sample_t a = 0.0f;
    sample_t gain = 0.01f;
    bool doNoise = false;
};



enum Params {
    GATE_THRESH,
    BITE,
    TIGHT,
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

const std::vector<juce::String> ParamIDs = {
    "GATE_THRESH",
    "BITE",
    "TIGHT",
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

    Biquad resonanceFilter {BIQUAD_LOWSHELF};
    Biquad presenceFilter {BIQUAD_HIGHSHELF};

	SmoothParamLinear masterVolume;

    TestOsc testOsc;

    double samplerate = 44100.0;

    sample_t *intputSignalCopy = nullptr;
    bool doTestOsc = false;

private:


    void initParameters();
    void parameterChanged(const juce::String &parameterID, float newValue) override;

	juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AmpModelerAudioProcessor)
};
