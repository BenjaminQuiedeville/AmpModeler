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
irloader 
    algorithme partitionné mono
    double IR mixable
    alignement de phase
    
général 
    bypass le bright cap et mettre un bright switch ?
    controller les attenuations en sortie des étages de gain ?
    
    presets
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

    GATE_ACTIVE,
    PREAMP_ACTIVE,
    TONESTACK_ACTIVE,
    EQ_ACTIVE,
    IR_ACTIVE,
    
    PREAMP_GAIN,
    BRIGHT_CAP,
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


    LOW_CUT_FREQ,
    LOW_SHELF_FREQ,
    LOW_SHELF_GAIN,
    LOWMID_FREQ,
    LOWMID_GAIN,
    LOWMID_Q,
    MID_FREQ,
    MID_GAIN,
    MID_Q,
    HIGH_FREQ,
    HIGH_GAIN,
    HIGH_Q,
    HIGH_SHELF_FREQ,
    HIGH_SHELF_GAIN,
    HIGH_CUT_FREQ,
    
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

    "GATE_ACTIVE",
    "PREAMP_ACTIVE",
    "TONESTACK_ACTIVE",
    "EQ_ACTIVE",
    "IR_ACTIVE",

    "PREAMP_GAIN",
    "BRIGHT_CAP",
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
    
    "LOW_CUT_FREQ",
    "LOW_SHELF_FREQ",
    "LOW_SHELF_GAIN",
    "LOWMID_FREQ",
    "LOWMID_GAIN",
    "LOWMID_Q",
    "MID_FREQ",
    "MID_GAIN",
    "MID_Q",
    "HIGH_FREQ",
    "HIGH_GAIN",
    "HIGH_Q",
    "HIGH_SHELF_FREQ",
    "HIGH_SHELF_GAIN",
    "HIGH_CUT_FREQ",
    
    "MASTER_VOLUME",
    "CHANNEL_CONFIG"
};

static const float defaultParamValues[N_PARAMS] = {
    0.0f,               // INPUT_GAIN
    
    -75.0f,             // GATE_THRESH
    1.0f,               // GATE_ATTACK
    15.0f,              // GATE_RELEASE
    0.0f,               // GATE_RETURN
    
    0.0f,               // SCREAMER_AMOUNT
    1300.0f,            // SCEAMER_FREQ
    10.0f,              // TIGHT

    1.0f,               // GATE_ACTIVE
    1.0f,               // PREAMP_ACTIVE
    1.0f,               // TONESTACK_ACTIVE
    1.0f,               // EQ_ACTIVE
    1.0f,               // IR_ACTIVE
    
    5.0f,               // PREAMP_GAIN
    1.0f,               // BRIGHT_CAP
    3.0f,               // CHANNEL 
    100.0f,             // INPUT_FILTER
    0.0f,               // PREAMP_VOLUME
    
    10000.0f,           // STAGE0_LP
    -5.0f,              // STAGE0_BYPASS
    0.0f,               // STAGE0_BIAS
    10.0f,              // STAGE1_HP
    18000.0f,           // STAGE1_LP
    -4.0f,               // STAGE1_BYPASS
    0.0f,               // STAGE1_BIAS
    10.0f,              // STAGE2_HP
    16000.0f,           // STAGE2_LP
    0.0f,               // STAGE2_BYPASS
    0.7f,               // STAGE2_BIAS
    10.0f,              // STAGE3_HP
    16000.0f,           // STAGE3_LP
    -6.0f,              // STAGE3_BYPASS
    0.0f,               // STAGE3_BIAS
    10.0f,              // STAGE4_HP
    16000.0f,           // STAGE4_LP
    -6.0f,              // STAGE4_BYPASS
    0.0f,               // STAGE4_BIAS
    
    0.0f,               // TONE_MODEL
    5.0f,               // TONE_BASS
    5.0f,               // TONE_MID
    5.0f,               // TONE_TREB

    5.0f,               // RESONANCE
    5.0f,               // PRESENCE

    0.0f,               // LOW_CUT_FREQ
    0.0f,               // LOW_SHELF_FREQ
    0.0f,               // LOW_SHELF_GAIN
    200.0f,             // LOWMID_FREQ
    0.0f,               // LOWMID_GAIN
    0.5f,               // LOWMID_Q
    700.0f,             // MID_FREQ
    0.0f,               // MID_GAIN
    0.5f,               // MID_Q
    2000.0f,            // HIGH_FREQ
    0.0f,               // HIGH_GAIN
    0.5f,               // HIGH_Q
    20000.0f,           // HIGH_SHELF_FREQ
    0.0f,               // HIGH_SHELF_GAIN
    20000.0f,           // HIGH_CUT_FREQ

    -3.0f,              // MASTER_VOLUME
    0.0f                // CHANNEL_CONFIG
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

    struct {
        Biquad lowCut    {BIQUAD_HIGHPASS};
        Biquad lowShelf  {BIQUAD_LOWSHELF};
        Biquad lowMid    {BIQUAD_PEAK};
        Biquad mid       {BIQUAD_PEAK};
        Biquad high      {BIQUAD_PEAK};
        Biquad highShelf {BIQUAD_HIGHSHELF};
        Biquad highCut   {BIQUAD_LOWPASS};
    } EQ;

    SmoothParamLinear inputGain;
    SmoothParamLinear masterVolume;

    double samplerate = 44100.0;
    double preampSamplerate = 44100.0f;

    float *sideChainBuffer = nullptr;
    
    u8 channelConfig = Mono;    
    bool gateActive = true;
    bool preampActive = true;
    bool tonestackActive = true;
    bool EQActive = true;
    
    void initParameters();
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
