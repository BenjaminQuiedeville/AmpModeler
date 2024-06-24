/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <assert.h>

//==============================================================================
Processor::Processor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
,
valueTree(irPathTree),
apvts(*this, nullptr, juce::Identifier("Params"), createParameterLayout())
{
    
    noiseGate = new NoiseGate();
    preamp    = new Preamp();
    toneStack = new Tonestack();
    irLoader  = new IRLoader();

    for (u8 i = 0; i < N_PARAMS; i++) {
        apvts.addParameterListener(ParamIDs[i], this);
    }
    
    valueTree.setProperty(irPath1, "C:/", nullptr);
    valueTree.setProperty(irPath2, "C:/", nullptr);
    
    apvts.state.appendChild(valueTree, nullptr);
}

Processor::~Processor() {

    for (u8 i = 0; i < N_PARAMS; i++) {
        apvts.removeParameterListener(ParamIDs[i], this);
    }

    delete noiseGate;
    delete preamp;
    delete toneStack;
    delete irLoader;

    if (sideChainBuffer) {
        free(sideChainBuffer);
    }    
}

//==============================================================================
const juce::String Processor::getName() const
{
    return JucePlugin_Name;
}

bool Processor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Processor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Processor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int Processor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Processor::getCurrentProgram()
{
    return 0;
}

void Processor::setCurrentProgram (int index)
{
    index;
    return;
}

const juce::String Processor::getProgramName (int index)
{
    index;
    return {};
}

void Processor::changeProgramName (int index, const juce::String& newName)
{
    newName; index;
    return;
}

//==============================================================================
void Processor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    samplerate = sampleRate;

    inputNoiseFilter.setCoefficients(2500.0, 0.7, 0.0, sampleRate);

    tightFilter.prepareToPlay();
    biteFilter.prepareToPlay();
    
    noiseGate->prepareToPlay(sampleRate);

    preamp->prepareToPlay(sampleRate, samplesPerBlock);

    masterVolume.init(0.0);

    
    toneStack->prepareToPlay(sampleRate);
    toneStack->setModel(EnglSavage);
    
    irLoader->init(sampleRate, samplesPerBlock);

    if (!sideChainBuffer) {
        sideChainBuffer = (Sample *)calloc(samplesPerBlock,  sizeof(Sample));
    }

    initParameters();
}

void Processor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Processor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{   
    midiMessages;

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    u32 numSamples = (u32)buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, (int)numSamples);
    }


    float *audioPtrL = buffer.getWritePointer(0);
    float *audioPtrR = buffer.getWritePointer(1);

    if (channelConfig == Mono) {
    
        audioPtrR = nullptr;
        inputNoiseFilter.processLeft(audioPtrL, numSamples);
    
        memcpy(sideChainBuffer, audioPtrL, numSamples * sizeof(Sample));
    
        /******PROCESS********/
        
        tightFilter.processHighpassLeft(audioPtrL, numSamples);
        biteFilter.processLeft(audioPtrL, numSamples);
    
        preamp->processStereo(audioPtrL, audioPtrR, numSamples);
        toneStack->processMono(audioPtrL, numSamples);
        
        resonanceFilter.processLeft(audioPtrL, numSamples);
        presenceFilter.processLeft(audioPtrL, numSamples);
        
        irLoader->process(audioPtrL, audioPtrR, numSamples);
        
        for (size_t i = 0; i < numSamples; i++) {
            audioPtrL[i] *= (Sample)dbtoa(masterVolume.nextValue());
        }
        
        noiseGate->process(audioPtrL, audioPtrR, sideChainBuffer, numSamples);

        if (channelConfig == Mono) {
            // copy left channel into right channel if processing is in mono
            buffer.copyFrom(1, 0, buffer, 0, 0, (int)numSamples);
        }

    } else {
    

        inputNoiseFilter.processStereo(audioPtrL, audioPtrR, numSamples);
    
        for (u32 i = 0; i < numSamples; i++) {
            sideChainBuffer[i] = (audioPtrL[i] + audioPtrR[i]) * 0.5f;
        }
    
        if (channelConfig == FakeStereo) {
            for (u32 i = 0; i < numSamples; i++) {
                audioPtrR[i] = -audioPtrL[i];
            }
        }
        /******PROCESS********/
        
        tightFilter.processHighpassStereo(audioPtrL, audioPtrR, numSamples);
        biteFilter.processStereo(audioPtrL, audioPtrR, numSamples);
    
        preamp->processStereo(audioPtrL, audioPtrR, numSamples);
        toneStack->processStereo(audioPtrL, audioPtrR, numSamples);
        
        resonanceFilter.processStereo(audioPtrL, audioPtrR, numSamples);
        presenceFilter.processStereo(audioPtrL, audioPtrR, numSamples);
        
        irLoader->process(audioPtrL, audioPtrR, numSamples);
        
        for (size_t i = 0; i < numSamples; i++) {
            Sample masterVolumeValue = (Sample)dbtoa(masterVolume.nextValue()); 
            audioPtrL[i] *= masterVolumeValue;
            audioPtrR[i] *= masterVolumeValue;
        }
        
        noiseGate->process(audioPtrL, audioPtrR, sideChainBuffer, numSamples);

        if (channelConfig == FakeStereo) {
            for (u32 i = 0; i < numSamples; i++) {
                audioPtrR[i] *= -1.0f;
            }
        }
    }

}

//==============================================================================
bool Processor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Processor::createEditor()
{       
    return new Editor (*this);
}

// @TODO utiliser juce::File, juce::FileOutputStream, juce::FileInputStream pour 
// stocker des preset dans %LOCALAPPDATA%/AmpModeler/presets
//==============================================================================
void Processor::getStateInformation (juce::MemoryBlock& destData) {
    // save params
    apvts.state.appendChild(valueTree, nullptr); 
    
    juce::MemoryOutputStream stream(destData, false);
    apvts.state.writeToStream(stream);
}   

void Processor::setStateInformation (const void* data, int sizeInBytes) {
    // Recall params
    juce::ValueTree importedTree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));

    if (importedTree.isValid()) {
        apvts.state = importedTree;
        valueTree = apvts.state.getChildWithName(irPathTree);
                
        juce::String irPath = valueTree.getProperty(irPath1);
        
        if (irPath.endsWith(".wav")){
            juce::File openedFile(irPath);
            irLoader->irFile = openedFile;
            irLoader->loadIR(false);
            DBG("ir file properly loaded from saved state");
        } else {
            irLoader->irFile = juce::File();
            irLoader->loadIR(true);
            DBG("could not load the stored ir file");
        }
        
        initParameters();
    }
}

void Processor::initParameters() {
    for (size_t i = 0; i < N_PARAMS; i++) {
        parameterChanged(ParamIDs[i].toString(), 
                         *apvts.getRawParameterValue(ParamIDs[i]));
    }
}

void Processor::parameterChanged(const juce::String &parameterId, float newValue) {


    const auto id = juce::Identifier(parameterId);

    if (id == ParamIDs[GATE_THRESH] 
        || id == ParamIDs[GATE_RETURN]) 
    {
        float thresh     = *apvts.getRawParameterValue(ParamIDs[GATE_THRESH]);
        float hysteresis = *apvts.getRawParameterValue(ParamIDs[GATE_RETURN]);
    
        noiseGate->threshold  = dbtoa(thresh);
        noiseGate->hysteresis = newValue;
        noiseGate->returnGain = dbtoa(thresh - hysteresis); 
            
        return;
    }
    
    if (id == ParamIDs[GATE_ATTACK]) {
        noiseGate->attackTimeMs = newValue;
        return;
    }
    
    if (id == ParamIDs[GATE_RELEASE]) {
        noiseGate->releaseTimeMs = newValue;
        return;
    }
    

    if (id == ParamIDs[BITE] || id == ParamIDs[BITE_FREQ]) {

        float amount = *apvts.getRawParameterValue(ParamIDs[BITE]);        
        float freq = *apvts.getRawParameterValue(ParamIDs[BITE_FREQ]);
    
        biteFilter.setCoefficients(freq, BOOST_BITE_Q, amount, samplerate);

        return;
    }

    if (id == ParamIDs[TIGHT]) {
        tightFilter.setCoefficients(newValue, samplerate);
        return;
    }

    if (id == ParamIDs[INPUT_FILTER]) {
        preamp->inputFilter.setCoefficients(newValue, samplerate*PREAMP_UP_SAMPLE_FACTOR);
        return;
    }

    if (id == ParamIDs[PREAMP_GAIN]) {
        auto paramRange = apvts.getParameter(id)->getNormalisableRange();

        preamp->preGain.newTarget(scale(newValue, paramRange.start, paramRange.end, 0.0f, 1.0f, 3.0f),
                                  SMOOTH_PARAM_TIME, 
                                  samplerate * PREAMP_UP_SAMPLE_FACTOR);
        preamp->brightCapFilter.setCoefficients(
            750.0, 
            scale_linear(newValue, paramRange.start, paramRange.end, -15.0f, 0.0f),
            samplerate*PREAMP_UP_SAMPLE_FACTOR
        );
        
        return;
    }

    if (id == ParamIDs[CHANNEL]) {
        
        preamp->channel = (u8)newValue;
        return;
    }

    if (id == ParamIDs[PREAMP_VOLUME]) {

        preamp->postGain.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate * PREAMP_UP_SAMPLE_FACTOR);

        return;
    }        

    if (id == ParamIDs[TONESTACK_BASS]
        || id == ParamIDs[TONESTACK_MIDDLE]
        || id == ParamIDs[TONESTACK_TREBBLE]
        || id == ParamIDs[TONESTACK_MODEL])
    {
        TonestackModel model = static_cast<TonestackModel>((int)*apvts.getRawParameterValue(ParamIDs[TONESTACK_MODEL]));
        
        if (toneStack->model != model) {
            toneStack->setModel(model);
        }
        
        float bassParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_BASS]);
        float trebbleParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_TREBBLE]);
        float midParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_MIDDLE]);
        toneStack->updateCoefficients(trebbleParam/10.0f, midParam/10.0f, bassParam/10.0f, samplerate);
        return;
    }

    if (id == ParamIDs[RESONANCE]) {
        resonanceFilter.setCoefficients(RESONANCE_FREQUENCY, scale_linear(newValue, 0.0f, 10.0f, 3.0f, 24.0f), samplerate);
        return;
    }

    if (id == ParamIDs[PRESENCE]) {
        presenceFilter.setCoefficients(PRESENCE_FREQUENCY, scale_linear(newValue, 0.0f, 10.0f, 0.0f, 18.0f), samplerate);
        return;
    }

    if (id == ParamIDs[MASTER_VOLUME]) {
        masterVolume.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }
    
    if (id == ParamIDs[CHANNEL_CONFIG]) {
        channelConfig = (u8)newValue;
        return;
    }
    
}

juce::AudioProcessorValueTreeState::ParameterLayout Processor::createParameterLayout()
{   
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    
    
    auto attributes = juce::AudioParameterFloatAttributes()
                        .withStringFromValueFunction ([] (auto x, auto) { return juce::String (x, 1); });
    
    // auto freqAttributes = juce::AudioParameterFloatAttributes()
    //                     .withStringFromValueFunction ([] (auto x, auto) { return juce::String (x, 1); });
    
    // auto dBAttributes = juce::AudioParameterFloatAttributes()
    //                     .withStringFromValueFunction ([] (auto x, auto) { return juce::String (x, 1); });
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_THRESH].toString(), "Gate Thresh", 
        juce::NormalisableRange<float>(-96.0f, -40.0f, 0.1f, 1.0f), -75.0f, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_ATTACK].toString(), "Attack Time", 
        juce::NormalisableRange<float>(0.1f, 15.0f, 0.1f, 1.0f), 1.0f, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_RELEASE].toString(), "Release Time", 
        juce::NormalisableRange<float>(1.0f, 25.0f, 0.1f, 1.0f), 15.0f, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_RETURN].toString(), "Hysteresis (not implemented)", 
        juce::NormalisableRange<float>(0.0f, 20.0f, 0.1f, 1.0f), 0.0f, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[BITE].toString(), "Bite", 
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f, 1.0f), 0.0f, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[BITE_FREQ].toString(), "Bite Freq", 
        juce::NormalisableRange<float>(500.0f, 2500.0f, 1.0f, 0.7f), 1200.0f, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TIGHT].toString(), "Tight", 
        juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.7f), 10.0f, attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_GAIN].toString(), "Pre Gain",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.05f, 1.0f), 5.0f, attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[INPUT_FILTER].toString(), "Input Filter", 
        juce::NormalisableRange<float>(10.0f, 1000.0f, 1.0f, 0.7f), 100.0f, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        ParamIDs[CHANNEL].toString(), "Amp Channel", 1, 4, 3
    ));
    

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_BASS].toString(), "Bass", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), 5.0f, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_MIDDLE].toString(), "Mid", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), 5.0f, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_TREBBLE].toString(), "Trebble", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), 5.0f, attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs[TONESTACK_MODEL].toString(), "Tonestack model", 
        juce::StringArray{"Savage", "JCM", "SLO", "Rect", "Orange"}, 0
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_VOLUME].toString(), "Post Gain", 
        juce::NormalisableRange<float>(-18.0f, 32.0f, 0.1f, 1.0f), 0.0f, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[RESONANCE].toString(), "Reson",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), 5.0f, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PRESENCE].toString(), "Presence",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), 5.0f, attributes
    ));


    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[MASTER_VOLUME].toString(), "Master Vol",
        juce::NormalisableRange<float>(-20.0f, 0.0f, 0.1f, 1.0f), -3.0f, attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs[CHANNEL_CONFIG].toString(), "Channel config",
        juce::StringArray{"Mono","Fake Stereo", "Stereo"}, 0
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}
