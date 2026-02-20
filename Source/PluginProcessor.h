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

#include "Biquad.h"

#include <vector>

#ifndef  JucePlugin_Name
    #define JucePlugin_Name "AmpModeler"
#endif

static const float BOOST_BITE_Q        = 0.25f;
static const float RESONANCE_FREQUENCY = 120.0f;
static const float PRESENCE_FREQUENCY  = 500.0f;

/*
TODO : 

    - normaliser les IRs et compenser le volume du mode sans IR
    - double IR mixable, alignement de phase
     
    - pouvoir mettre les 2 premiers etages de gains en parallele à la plexi
    - ajouter tablesaw
    - désactiver le boost et la gate indépendament  
    - refaire la gate 
    - mieux indiquer la correspondance entre les gains 1, 2 du panneaux principal et les 
    gain 1, 2 du panneau de gain stage
    - les boutons de désactivation sur leur panneau respectif
    - flow diagram pour les étages de gain 
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
    BOOST_ACTIVE,
    PREAMP_ACTIVE,
    TONESTACK_ACTIVE,
    EQ_ACTIVE,
    IR_ACTIVE,
    
    BRIGHT_CAP,
    CHANNEL,
    INPUT_FILTER,
    PREAMP_VOLUME,
    
    STAGE1_LP,
    STAGE1_BYPASS,
    STAGE1_BIAS,
    GAIN1,
    
    STAGE2_LP,
    STAGE2_BYPASS,
    STAGE2_BIAS,
    GAIN2,
    
    STAGE3_LP,
    STAGE3_BYPASS,
    STAGE3_BIAS,
    GAIN3,
    
    STAGE4_LP,
    STAGE4_BYPASS,
    STAGE4_BIAS,
    GAIN4,
        
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

struct ParameterInfo {
    juce::Identifier id;
    float defaultValue = 0.0f;
};

static const ParameterInfo paramInfos[N_PARAMS] {
    { "INPUT_GAIN",           0.0f },
    
    { "GATE_THRESH",          -75.0f },
    { "GATE_ATTACK",          1.0f },
    { "GATE_RELEASE",         15.0f },
    { "GATE_RETURN",          0.0f },
    
    { "SCREAMER_AMOUNT",      0.0f },
    { "SCEAMER_FREQ",         1300.0f },
    { "TIGHT",                10.0f },
    
    { "GATE_ACTIVE",          1.0f },
    { "BOOST_ACTIVE",         1.0f },
    { "PREAMP_ACTIVE",        1.0f },
    { "TONESTACK_ACTIVE",     1.0f },
    { "EQ_ACTIVE",            0.0f },
    { "IR_ACTIVE",            1.0f },
    
    { "BRIGHT_CAP",           1.0f },
    { "CHANNEL",              3.0f },
    { "INPUT_FILTER",         100.0f },
    { "PREAMP_VOLUME",        0.0f },
    
    { "STAGE1_LP",            15000.0f },
    { "STAGE1_BYPASS",        -4.0f },
    { "STAGE1_BIAS",          0.0f },
    { "GAIN1",                5.0f },
    
    { "STAGE2_LP",            15000.0f },
    { "STAGE2_BYPASS",        -3.0f },
    { "STAGE2_BIAS",          0.1f },
    { "GAIN2",                5.0f },
    
    { "STAGE3_LP",            15000.0f },
    { "STAGE3_BYPASS",        0.0f },
    { "STAGE3_BIAS",          0.3f },
    { "GAIN3",                9.0f },
    
    { "STAGE4_LP",            15000.0f },
    { "STAGE4_BYPASS",        -2.0f },
    { "STAGE4_BIAS",          0.0f },
    { "GAIN4",                5.0f },
    
    { "TONE_MODEL",           0.0f },
    { "TONE_BASS",            5.0f },
    { "TONE_MID",             5.0f },
    { "TONE_TREB",            5.0f },
    
    { "RESONANCE",            5.0f },
    { "PRESENCE",             5.0f },
    
    { "LOW_CUT_FREQ",         0.0f },
    { "LOW_SHELF_FREQ",       0.0f },
    { "LOW_SHELF_GAIN",       0.0f },
    { "LOWMID_FREQ",          200.0f },
    { "LOWMID_GAIN",          0.0f },
    { "LOWMID_Q",             0.5f },
    { "MID_FREQ",             700.0f },
    { "MID_GAIN",             0.0f },
    { "MID_Q",                0.5f },
    { "HIGH_FREQ",            2000.0f },
    { "HIGH_GAIN",            0.0f },
    { "HIGH_Q",               0.5f },
    { "HIGH_SHELF_FREQ",      20000.0f },
    { "HIGH_SHELF_GAIN",      0.0f },
    { "HIGH_CUT_FREQ",        20000.0f },
    
    { "MASTER_VOLUME",        0.0f },
    { "CHANNEL_CONFIG",       0.0f }
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

    Biquad inputNoiseFilter;

    FirstOrderFilter tightFilter;
    Biquad biteFilter;

    NoiseGate noiseGate;
    Preamp preamp;
    Tonestack toneStack;
    IRLoader irLoader;

    FirstOrderFilter resonanceFilter;
    FirstOrderFilter presenceFilter;

    struct {
        Biquad lowCut;
        Biquad lowShelf;
        Biquad lowMid;
        Biquad mid;
        Biquad high;
        Biquad highShelf;
        Biquad highCut;
    } EQ;

    SmoothParamLinear inputGain;
    SmoothParamLinear masterVolume;

    float samplerate = 48000.0f;
    float preampSamplerate = 48000.0f;
    int bufferSize = 64;

    float *sideChainBuffer = nullptr;
    
    u8 channelConfig = Mono;    
    bool doGate = true;
    bool doBoost = true;
    bool doPreamp = true;
    bool doTonestack = true;
    bool doEQ = false;
    
    void initParameters();
    void parameterChanged(const juce::String &parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
