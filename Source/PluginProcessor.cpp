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
    
    for (u8 i = 0; i < N_PARAMS; i++) {
        apvts.addParameterListener(ParamIDs[i], this);
    }
    
    valueTree.setProperty(irPath1, "", nullptr);
    valueTree.setProperty(irPath2, "", nullptr);
    
    apvts.state.appendChild(valueTree, nullptr);
}

Processor::~Processor() {

    for (u8 i = 0; i < N_PARAMS; i++) {
        apvts.removeParameterListener(ParamIDs[i], this);
    }

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
    
    noiseGate.prepareToPlay(sampleRate);

    preamp.prepareToPlay(sampleRate, samplesPerBlock);
    
    inputGain.init(0.0);
    masterVolume.init(0.0);
    
    toneStack.setModel(EnglSavage); // change to current selected model
    toneStack.prepareToPlay(sampleRate);
    
    irLoader.init(sampleRate, samplesPerBlock);

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
    }

    if (channelConfig == FakeStereo) {
        for (u32 i = 0; i < numSamples; i++) {
            audioPtrR[i] = -audioPtrL[i];
        }
    }


    inputGain.applySmoothGainDeciBels(audioPtrL, audioPtrR, numSamples);

    inputNoiseFilter.process(audioPtrL, audioPtrR, numSamples);

    if (channelConfig == Stereo) {
        for (u32 i = 0; i < numSamples; i++) {
            sideChainBuffer[i] = (audioPtrL[i] + audioPtrR[i]) * 0.5f;
        }
    } else {
        for (u32 i = 0; i < numSamples; i++) {
            sideChainBuffer[i] = audioPtrL[i];
        }
    }
    
    /******PROCESS********/
    if (gateActive) {
        tightFilter.processHighpass(audioPtrL, audioPtrR, numSamples);
        biteFilter.process(audioPtrL, audioPtrR, numSamples);
    }
    
    if (preampActive) {
        preamp.process(audioPtrL, audioPtrR, numSamples);
    }
    
    if (tonestackActive) {
        toneStack.process(audioPtrL, audioPtrR, numSamples);
    }
    
    resonanceFilter.process(audioPtrL, audioPtrR, numSamples);
    presenceFilter.process(audioPtrL, audioPtrR, numSamples);
        
    irLoader.process(audioPtrL, audioPtrR, numSamples);

    EQ.lowCut.process(audioPtrL, audioPtrR, numSamples);
    EQ.lowShelf.process(audioPtrL, audioPtrR, numSamples);
    EQ.lowMid.process(audioPtrL, audioPtrR, numSamples);
    EQ.mid.process(audioPtrL, audioPtrR, numSamples);
    EQ.high.process(audioPtrL, audioPtrR, numSamples);
    EQ.highShelf.process(audioPtrL, audioPtrR, numSamples);
    EQ.highCut.process(audioPtrL, audioPtrR, numSamples);

    masterVolume.applySmoothGainDeciBels(audioPtrL, audioPtrR, numSamples);
        
    if (gateActive) {
        noiseGate.process(audioPtrL, audioPtrR, sideChainBuffer, numSamples);
    }

    if (channelConfig == Mono) {
        // copy left channel into right channel if processing is in mono
        buffer.copyFrom(1, 0, buffer, 0, 0, (int)numSamples);
    }

    if (channelConfig == FakeStereo) {
        for (u32 i = 0; i < numSamples; i++) {
            audioPtrR[i] *= -1.0f;
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
                
        initParameters();
        
        juce::String irPath = valueTree.getProperty(irPath1);
        
        if (irPath.endsWith(".wav")){
            juce::File openedFile(irPath);
            irLoader.irFile = openedFile;
            irLoader.loadIR();
            
            // get the list of files in the same directory
            juce::File parentFolder = openedFile.getParentDirectory();
            irLoader.directoryWavFiles = parentFolder.findChildFiles(
                juce::File::TypesOfFileToFind::findFiles,
                false, 
                "*.wav",
                juce::File::FollowSymlinks::no
            );

            irLoader.indexOfCurrentFile = irLoader.directoryWavFiles.indexOf(openedFile);            
            DBG("ir file properly loaded from saved state");

        } else {
            irLoader.irFile = juce::File();
            irLoader.loadIR();
            DBG("could not load the stored ir file");
        }
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
    
        noiseGate.threshold  = dbtoa(thresh);
        noiseGate.hysteresis = newValue;
        noiseGate.returnGain = dbtoa(thresh - hysteresis); 
            
        return;
    }

    if (id == ParamIDs[INPUT_GAIN]) {
        inputGain.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }

    
    if (id == ParamIDs[GATE_ATTACK]) {
        noiseGate.attackTimeMs = newValue;
        return;
    }
    
    if (id == ParamIDs[GATE_RELEASE]) {
        noiseGate.releaseTimeMs = newValue;
        return;
    }
    

    if (id == ParamIDs[SCREAMER_AMOUNT] || id == ParamIDs[SCREAMER_FREQ]) {

        float amount = *apvts.getRawParameterValue(ParamIDs[SCREAMER_AMOUNT]);        
        float freq = *apvts.getRawParameterValue(ParamIDs[SCREAMER_FREQ]);
    
        biteFilter.setCoefficients(freq, BOOST_BITE_Q, amount, samplerate);

        return;
    }

    if (id == ParamIDs[TIGHT]) {
        tightFilter.setCoefficients(newValue, samplerate);
        return;
    }

    if (id == ParamIDs[INPUT_FILTER]) {
        preamp.inputFilter.setCoefficients(newValue, samplerate*PREAMP_UP_SAMPLE_FACTOR);
        return;
    }

    if (id == ParamIDs[PREAMP_GAIN]) {
        auto paramRange = apvts.getParameter(id)->getNormalisableRange();

        preamp.preGain.newTarget(scale(newValue, paramRange.start, paramRange.end, 0.0f, 1.0f, 3.0f),
                                  SMOOTH_PARAM_TIME, 
                                  samplerate * PREAMP_UP_SAMPLE_FACTOR);
        preamp.brightCapFilter.setCoefficients(
            450.0, 
            scale_linear(newValue, paramRange.start, paramRange.end, -15.0f, 0.0f),
            samplerate*PREAMP_UP_SAMPLE_FACTOR
        );
        
        return;
    }

    if (id == ParamIDs[CHANNEL]) {
        preamp.channel = (u8)newValue;
        return;
    }

    if (id == ParamIDs[PREAMP_VOLUME]) {
        preamp.postGain.newTarget(newValue + preamp.outputAttenuationdB, SMOOTH_PARAM_TIME, samplerate * PREAMP_UP_SAMPLE_FACTOR);
        return;
    }
    
    if (id == ParamIDs[STAGE0_LP]
        || id == ParamIDs[STAGE0_BYPASS]
        || id == ParamIDs[STAGE0_BIAS])
    {
        preamp.stageOutputFilter0.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE0_LP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);
        
        preamp.cathodeBypassFilter0.setCoefficients(250.0, 
            *apvts.getRawParameterValue(ParamIDs[STAGE0_BYPASS]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.setBias(*apvts.getRawParameterValue(ParamIDs[STAGE0_BIAS]), 0);
        
        return;
    }
    
    if (id == ParamIDs[STAGE1_HP]
        || id == ParamIDs[STAGE1_LP]
        || id == ParamIDs[STAGE1_BYPASS]
        || id == ParamIDs[STAGE1_BIAS])
    {
    
        preamp.couplingFilter1.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE1_HP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.stageOutputFilter1.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE1_LP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);
        
        preamp.cathodeBypassFilter1.setCoefficients(250.0, 
            *apvts.getRawParameterValue(ParamIDs[STAGE1_BYPASS]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.setBias(*apvts.getRawParameterValue(ParamIDs[STAGE1_BIAS]), 1);
        return;
    }
    
    if (id == ParamIDs[STAGE2_HP]
        || id == ParamIDs[STAGE2_LP]
        || id == ParamIDs[STAGE2_BYPASS]
        || id == ParamIDs[STAGE2_BIAS])
    {
        preamp.couplingFilter2.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE2_HP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.stageOutputFilter2.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE2_LP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);
        
        preamp.cathodeBypassFilter2.setCoefficients(250.0, 
            *apvts.getRawParameterValue(ParamIDs[STAGE2_BYPASS]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.setBias(*apvts.getRawParameterValue(ParamIDs[STAGE2_BIAS]), 2);
        return;
    }
    
    if (id == ParamIDs[STAGE3_HP]
        || id == ParamIDs[STAGE3_LP]
        || id == ParamIDs[STAGE3_BYPASS]
        || id == ParamIDs[STAGE3_BIAS])
    {
        preamp.couplingFilter3.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE3_HP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.stageOutputFilter3.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE3_LP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);
        
        preamp.cathodeBypassFilter3.setCoefficients(250.0, 
            *apvts.getRawParameterValue(ParamIDs[STAGE3_BYPASS]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.setBias(*apvts.getRawParameterValue(ParamIDs[STAGE3_BIAS]), 3);
        return;
    }
    
    if (id == ParamIDs[STAGE4_HP]
        || id == ParamIDs[STAGE4_LP]
        || id == ParamIDs[STAGE4_BYPASS]
        || id == ParamIDs[STAGE4_BIAS])
    {
        preamp.couplingFilter4.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE4_HP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.stageOutputFilter4.setCoefficients(
            *apvts.getRawParameterValue(ParamIDs[STAGE4_LP]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);
        
        preamp.cathodeBypassFilter4.setCoefficients(250.0, 
            *apvts.getRawParameterValue(ParamIDs[STAGE4_BYPASS]), 
            samplerate*PREAMP_UP_SAMPLE_FACTOR);

        preamp.setBias(*apvts.getRawParameterValue(ParamIDs[STAGE4_BIAS]), 4);
        return;
    }
    
    if (id == ParamIDs[TONESTACK_BASS]
        || id == ParamIDs[TONESTACK_MIDDLE]
        || id == ParamIDs[TONESTACK_TREBBLE]
        || id == ParamIDs[TONESTACK_MODEL])
    {
        TonestackModel model = static_cast<TonestackModel>((int)*apvts.getRawParameterValue(ParamIDs[TONESTACK_MODEL]));
        
        if (toneStack.model != model) {
            toneStack.setModel(model);
        }
        
        float bassParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_BASS]);
        float trebbleParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_TREBBLE]);
        float midParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_MIDDLE]);
        toneStack.updateCoefficients(trebbleParam/10.0f, midParam/10.0f, bassParam/10.0f, samplerate);
        return;
    }

    if (id == ParamIDs[RESONANCE]) {
        resonanceFilter.setCoefficients(RESONANCE_FREQUENCY, scale_linear(newValue, 0.0f, 10.0f, 0.0f, 24.0f), samplerate);
        return;
    }

    if (id == ParamIDs[PRESENCE]) {
        presenceFilter.setCoefficients(PRESENCE_FREQUENCY, scale_linear(newValue, 0.0f, 10.0f, 0.0f, 18.0f), samplerate);
        return;
    }
    
    if (id == ParamIDs[LOW_CUT_FREQ]) {
        
        EQ.lowCut.setCoefficients(newValue, 0.7, 0.0, samplerate);
        return;
    }
    
    if (id == ParamIDs[LOW_SHELF_FREQ]
        || id == ParamIDs[LOW_SHELF_GAIN])
    {
        
        EQ.lowShelf.setCoefficients(*apvts.getRawParameterValue(ParamIDs[LOW_SHELF_FREQ]), 0.7,
                                    *apvts.getRawParameterValue(ParamIDs[LOW_SHELF_GAIN]), samplerate);
        return;
    }
    
    if (id == ParamIDs[LOWMID_FREQ]
        || id == ParamIDs[LOWMID_GAIN]
        || id == ParamIDs[LOWMID_Q])
    {
    
        EQ.lowMid.setCoefficients(*apvts.getRawParameterValue(ParamIDs[LOWMID_FREQ]),
                                  *apvts.getRawParameterValue(ParamIDs[LOWMID_Q]),
                                  *apvts.getRawParameterValue(ParamIDs[LOWMID_GAIN]), samplerate);
        return;
    }
    
    if (id == ParamIDs[MID_FREQ]
        || id == ParamIDs[MID_GAIN]
        || id == ParamIDs[MID_Q])
    {
        EQ.mid.setCoefficients(*apvts.getRawParameterValue(ParamIDs[MID_FREQ]),
                               *apvts.getRawParameterValue(ParamIDs[MID_Q]),
                               *apvts.getRawParameterValue(ParamIDs[MID_GAIN]), samplerate);
        return;
    }
    
    if (id == ParamIDs[HIGH_FREQ]
        || id == ParamIDs[HIGH_GAIN]
        || id == ParamIDs[HIGH_Q])
    {
        EQ.high.setCoefficients(*apvts.getRawParameterValue(ParamIDs[HIGH_FREQ]),
                                *apvts.getRawParameterValue(ParamIDs[HIGH_Q]),
                                *apvts.getRawParameterValue(ParamIDs[HIGH_GAIN]), samplerate);
        return;
    }
    
    if (id == ParamIDs[HIGH_SHELF_FREQ]
        || id == ParamIDs[HIGH_SHELF_GAIN])
    {
        EQ.highShelf.setCoefficients(*apvts.getRawParameterValue(ParamIDs[HIGH_SHELF_FREQ]), 0.7,
                                     *apvts.getRawParameterValue(ParamIDs[HIGH_SHELF_GAIN]), samplerate);
        return;
    }
    
    if (id == ParamIDs[HIGH_CUT_FREQ]) {        
        EQ.highCut.setCoefficients(*apvts.getRawParameterValue(ParamIDs[HIGH_CUT_FREQ]), 0.7, 0.0, samplerate);
        return;
    }
    
    if (id == ParamIDs[BYPASS_IR]) {
        irLoader.active = (bool)newValue;
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
        ParamIDs[INPUT_GAIN].toString(), "Input Gain", 
        juce::NormalisableRange<float>(-36.0f, 12.0f, 0.1f, 1.0f), defaultParamValues[INPUT_GAIN], attributes
    ));

    // Noise gate params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_THRESH].toString(), "Gate Thresh", 
        juce::NormalisableRange<float>(-96.0f, -40.0f, 0.1f, 1.0f), defaultParamValues[GATE_THRESH], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_ATTACK].toString(), "Attack Time", 
        juce::NormalisableRange<float>(0.1f, 15.0f, 0.1f, 1.0f), defaultParamValues[GATE_ATTACK], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_RELEASE].toString(), "Release Time", 
        juce::NormalisableRange<float>(1.0f, 25.0f, 0.1f, 1.0f), defaultParamValues[GATE_RELEASE], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_RETURN].toString(), "Hysteresis (not implemented)", 
        juce::NormalisableRange<float>(0.0f, 20.0f, 0.1f, 1.0f), defaultParamValues[GATE_RETURN], attributes
    ));
    
    // Input Boost params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[SCREAMER_AMOUNT].toString(), "Screamer", 
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f, 1.0f), defaultParamValues[SCREAMER_AMOUNT], attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[SCREAMER_FREQ].toString(), "Screamer Freq", 
        juce::NormalisableRange<float>(500.0f, 2500.0f, 1.0f, 0.7f), defaultParamValues[SCREAMER_FREQ], attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TIGHT].toString(), "Tight", 
        juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.7f), defaultParamValues[TIGHT], attributes
    ));

    // Preamp Params 
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_GAIN].toString(), "Pre Gain",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.05f, 1.0f), defaultParamValues[PREAMP_GAIN], attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[INPUT_FILTER].toString(), "Input Filter", 
        juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.7f), defaultParamValues[INPUT_FILTER], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        ParamIDs[CHANNEL].toString(), "Amp Channel", 1, 4, (int)defaultParamValues[CHANNEL]
    ));
    
    // Tubes params    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE0_LP].toString(), "STAGE0_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), defaultParamValues[STAGE0_LP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE0_BYPASS].toString(), "STAGE0_BYPASS", 
        juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f, 1.0f),  defaultParamValues[STAGE0_BYPASS], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE0_BIAS].toString(), "STAGE0_BIAS", 
        juce::NormalisableRange<float>(0.0, 1.0f, 0.01f, 1.0f), defaultParamValues[STAGE0_BIAS], attributes
    ));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE1_HP].toString(), "STAGE1_HP", 
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f, 0.7f), defaultParamValues[STAGE1_HP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE1_LP].toString(), "STAGE1_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), defaultParamValues[STAGE1_LP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE1_BYPASS].toString(), "STAGE1_BYPASS", 
        juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f, 1.0f), defaultParamValues[STAGE1_BYPASS], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE1_BIAS].toString(), "STAGE1_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), defaultParamValues[STAGE1_BIAS], attributes
    ));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE2_HP].toString(), "STAGE2_HP", 
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f, 0.7f), defaultParamValues[STAGE2_HP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE2_LP].toString(), "STAGE2_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), defaultParamValues[STAGE2_LP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE2_BYPASS].toString(), "STAGE2_BYPASS", 
        juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f, 1.0f),  defaultParamValues[STAGE2_BYPASS], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE2_BIAS].toString(), "STAGE2_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f, 1.0f), defaultParamValues[STAGE2_BIAS], attributes
    ));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE3_HP].toString(), "STAGE3_HP", 
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f, 0.7f), defaultParamValues[STAGE3_HP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE3_LP].toString(), "STAGE3_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f, 0.7f), defaultParamValues[STAGE3_LP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE3_BYPASS].toString(), "STAGE3_BYPASS", 
        juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f, 1.0f),  defaultParamValues[STAGE3_BYPASS], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE3_BIAS].toString(), "STAGE3_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f, 1.0f), defaultParamValues[STAGE3_BIAS], attributes
    ));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE4_HP].toString(), "STAGE4_HP", 
        juce::NormalisableRange<float>(0.0f, 50.0f, 0.1f, 0.7f), defaultParamValues[STAGE4_HP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE4_LP].toString(), "STAGE4_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), defaultParamValues[STAGE4_LP], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE4_BYPASS].toString(), "STAGE4_BYPASS", 
        juce::NormalisableRange<float>(-6.0f, 0.0f, 0.1f, 1.0f), defaultParamValues[STAGE4_BYPASS], attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[STAGE4_BIAS].toString(), "STAGE4_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.1f, 1.0f), defaultParamValues[STAGE4_BIAS], attributes
    ));
    
    
    // Tonestack params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_BASS].toString(), "Bass", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), defaultParamValues[TONESTACK_BASS], attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_MIDDLE].toString(), "Mid", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), defaultParamValues[TONESTACK_MIDDLE], attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_TREBBLE].toString(), "Trebble", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), defaultParamValues[TONESTACK_TREBBLE], attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs[TONESTACK_MODEL].toString(), "Tonestack model", 
        juce::StringArray{"Savage", "JCM", "SLO", "Rect", "Orange"}, (int)defaultParamValues[TONESTACK_MODEL]
    ));


    // Post Preamp params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_VOLUME].toString(), "Post Gain", 
        juce::NormalisableRange<float>(-36.0f, 36.0f, 0.1f, 1.0f), defaultParamValues[PREAMP_VOLUME], attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[RESONANCE].toString(), "Resonance",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), defaultParamValues[RESONANCE], attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PRESENCE].toString(), "Presence",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), defaultParamValues[PRESENCE], attributes
    ));

    // EQ params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[LOW_CUT_FREQ].toString(), "",
        juce::NormalisableRange<float>(10.0f, 200.0f, 0.1f, 0.7f), defaultParamValues[LOW_CUT_FREQ], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[LOW_SHELF_FREQ].toString(), "",
        juce::NormalisableRange<float>(10.0f, 250.0f, 0.1f, 0.7f), defaultParamValues[LOW_SHELF_FREQ], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[LOW_SHELF_GAIN].toString(), "",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), defaultParamValues[LOW_SHELF_GAIN], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[LOWMID_FREQ].toString(), "",
        juce::NormalisableRange<float>(100.0f, 500.0f, 0.1f, 0.7f), defaultParamValues[LOWMID_FREQ], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[LOWMID_GAIN].toString(), "",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), defaultParamValues[LOWMID_GAIN], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[LOWMID_Q].toString(), "",
        juce::NormalisableRange<float>(0.1f, 1.5f, 0.01f, 1.0f), defaultParamValues[LOWMID_Q]
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[MID_FREQ].toString(), "",
        juce::NormalisableRange<float>(400.0f, 2000.0f, 1.0f, 0.7f), defaultParamValues[MID_FREQ], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[MID_GAIN].toString(), "",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), defaultParamValues[MID_GAIN], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[MID_Q].toString(), "",
        juce::NormalisableRange<float>(0.1f, 1.5f, 0.01f, 1.0f), defaultParamValues[MID_Q]
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[HIGH_FREQ].toString(), "",
        juce::NormalisableRange<float>(1000.0f, 8000.0f, 1.0f, 0.7f), defaultParamValues[HIGH_FREQ], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[HIGH_GAIN].toString(), "",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), defaultParamValues[HIGH_GAIN], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[HIGH_Q].toString(), "",
        juce::NormalisableRange<float>(0.1f, 1.5f, 0.01f, 1.0f), defaultParamValues[HIGH_Q]
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[HIGH_SHELF_FREQ].toString(), "",
        juce::NormalisableRange<float>(4000.0f, 20000.0f, 1.0f, 0.7f), defaultParamValues[HIGH_SHELF_FREQ], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[HIGH_SHELF_GAIN].toString(), "",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), defaultParamValues[HIGH_SHELF_GAIN], attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[HIGH_CUT_FREQ].toString(), "",
        juce::NormalisableRange<float>(4000.0f, 20000.0f, 1.0f, 0.7f), defaultParamValues[HIGH_CUT_FREQ], attributes
    ));
        


    params.push_back(std::make_unique<juce::AudioParameterBool>(
        ParamIDs[BYPASS_IR].toString(), "Activate IR Loader", 
        (bool)defaultParamValues[BYPASS_IR]
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[MASTER_VOLUME].toString(), "Master Vol",
        juce::NormalisableRange<float>(-18.0f, 12.0f, 0.1f, 1.0f), defaultParamValues[MASTER_VOLUME], attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs[CHANNEL_CONFIG].toString(), "Channel config",
        juce::StringArray{"Mono","Fake Stereo", "Stereo"}, (int)defaultParamValues[CHANNEL_CONFIG]
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}
