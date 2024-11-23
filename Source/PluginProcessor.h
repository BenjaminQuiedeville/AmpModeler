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

const float BOOST_BITE_Q        = 0.25f;

const float RESONANCE_FREQUENCY = 120.0f;
const float PRESENCE_FREQUENCY  = 500.0f;

/*
TODO : 

parametric EQ panel

tube parameters : reset button

général 
    presets

tonestack
    composants custom
    
irloader 

    double IR mixable
    alignement de phase

    algorithme partitionné
*/

enum Params {
    
    INPUT_GAIN,
    
    GATE_THRESH,
    GATE_ATTACK,
    GATE_RELEASE,
    GATE_RETURN,
    
    SCREAMER_AMOUNT,
    SCREAMER_FREQ,
    TIGHT,
    
    PREAMP_GAIN,
    CHANNEL,
    INPUT_FILTER,
    PREAMP_VOLUME,
    
    STAGE0_LP,
    STAGE0_BYPASS,
    STAGE0_BIAS,
    
    STAGE1_HP,
    STAGE1_LP,
    STAGE1_BYPASS,
    STAGE1_BIAS,
    
    STAGE2_HP,
    STAGE2_LP,
    STAGE2_BYPASS,
    STAGE2_BIAS,
    
    STAGE3_HP,
    STAGE3_LP,
    STAGE3_BYPASS,
    STAGE3_BIAS,
    
    STAGE4_HP,
    STAGE4_LP,
    STAGE4_BYPASS,
    STAGE4_BIAS,
    
    TONESTACK_MODEL,
    TONESTACK_BASS,
    TONESTACK_MIDDLE,
    TONESTACK_TREBBLE,
    
    RESONANCE,
    PRESENCE,
    
    BYPASS_IR,
    MASTER_VOLUME,
    CHANNEL_CONFIG,
    N_PARAMS
};


static std::vector<juce::Identifier> ParamIDs = {
    "INPUT_GAIN",

    "GATE_THRESH",
    "GATE_ATTACK",
    "GATE_RELEASE",
    "GATE_RETURN",
    
    "SCREAMER_AMOUNT",
    "SCEAMER_FREQ",
    "TIGHT",
    
    "PREAMP_GAIN",
    "CHANNEL",
    "INPUT_FILTER",
    "PREAMP_VOLUME",
    
    "STAGE0_LP",
    "STAGE0_BYPASS",
    "STAGE0_BIAS",
    
    "STAGE1_HP",
    "STAGE1_LP",
    "STAGE1_BYPASS",
    "STAGE1_BIAS",
    
    "STAGE2_HP",
    "STAGE2_LP",
    "STAGE2_BYPASS",
    "STAGE2_BIAS",
    
    "STAGE3_HP",
    "STAGE3_LP",
    "STAGE3_BYPASS",
    "STAGE3_BIAS",
    
    "STAGE4_HP",
    "STAGE4_LP",
    "STAGE4_BYPASS",
    "STAGE4_BIAS",
    
    "TONE_MODEL",
    "TONE_BASS",
    "TONE_MID",
    "TONE_TREB",
    
    "RESONANCE",
    "PRESENCE",
    
    "BYPASS_IR",
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

    NoiseGate noiseGate;
    Preamp preamp;
    Tonestack toneStack;
    IRLoader irLoader;

    FirstOrderShelfFilter resonanceFilter {lowshelf};
    FirstOrderShelfFilter presenceFilter {highshelf};

    SmoothParamLinear inputGain;
    SmoothParamLinear masterVolume;

    double samplerate = 44100.0;

    Sample *sideChainBuffer = nullptr;
    
    u8 channelConfig = Mono;    
    bool gateActive = true;
    bool preampActive = true;
    bool tonestackActive = true;

    void initParameters();
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
