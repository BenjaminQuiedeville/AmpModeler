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
        apvts.addParameterListener(paramInfos[i].id, this);
    }
    
    valueTree.setProperty(irPath1, "", nullptr);
    valueTree.setProperty(irPath2, "", nullptr);
    
    apvts.state.appendChild(valueTree, nullptr);
}

Processor::~Processor() {

    for (u8 i = 0; i < N_PARAMS; i++) {
        apvts.removeParameterListener(paramInfos[i].id, this);
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
void Processor::prepareToPlay (double sampleRate, int samplesPerBlock) {
    ZoneScoped;
    
    samplerate = sampleRate;
    bufferSize = samplesPerBlock;
    preampSamplerate = samplerate * PREAMP_UP_SAMPLE_FACTOR;
    inputNoiseFilter.setCoefficients(3000.0, 0.7, 0.0, samplerate);

    tightFilter.prepareToPlay();
    biteFilter.prepareToPlay();
    
    noiseGate.prepareToPlay(samplerate);

    preamp.prepareToPlay(samplerate, bufferSize);
    
    inputGain.init(0.0);
    masterVolume.init(0.0);
    
    toneStack.samplerate = samplerate;
    toneStack.setModel(EnglSavage); // change to current selected model
    toneStack.prepareToPlay();
    irLoader.init(samplerate, bufferSize);

    if (!sideChainBuffer) {
        sideChainBuffer = (float *)calloc(bufferSize,  sizeof(float));
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

void Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {   
    ZoneScoped;
    
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
    assert(audioPtrL && "processBlock() : audioPtrL is Null");
    assert(audioPtrR && "processBlock() : audioPtrR is Null");

    u8 currentChannelConfig = channelConfig;

    if (currentChannelConfig == Mono) { 
        audioPtrR = nullptr;
    }

    if (currentChannelConfig == FakeStereo) {
        for (u32 i = 0; i < numSamples; i++) {
            audioPtrR[i] = -audioPtrL[i];
        }
    }


    inputGain.applySmoothGainDeciBels(audioPtrL, audioPtrR, numSamples);
    
    // if (gateActive) {
    //     inputNoiseFilter.process(audioPtrL, audioPtrR, numSamples);
    // }
    
    if (currentChannelConfig == Stereo) {
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
    
    if (preampActive) {
        resonanceFilter.process(audioPtrL, audioPtrR, numSamples);
        presenceFilter.process(audioPtrL, audioPtrR, numSamples);
    }
        
    if (irLoader.active) {
        irLoader.process(audioPtrL, audioPtrR, numSamples);
    }
    if (EQActive) {
        EQ.lowCut.process(audioPtrL, audioPtrR, numSamples);
        EQ.lowShelf.process(audioPtrL, audioPtrR, numSamples);
        EQ.lowMid.process(audioPtrL, audioPtrR, numSamples);
        EQ.mid.process(audioPtrL, audioPtrR, numSamples);
        EQ.high.process(audioPtrL, audioPtrR, numSamples);
        EQ.highShelf.process(audioPtrL, audioPtrR, numSamples);
        EQ.highCut.process(audioPtrL, audioPtrR, numSamples);
    }

    masterVolume.applySmoothGainDeciBels(audioPtrL, audioPtrR, numSamples);
        
    if (gateActive) {
        noiseGate.process(audioPtrL, audioPtrR, sideChainBuffer, numSamples);
    }

    for (u32 index = 0; index < numSamples; index++) {
        audioPtrL[index] = CLIP(audioPtrL[index], -1.0f, 1.0f);
    }
    
    if (audioPtrR) {
        for (u32 index = 0; index < numSamples; index++) {
            audioPtrR[index] = CLIP(audioPtrR[index], -1.0f, 1.0f);
        }
    }

    if (currentChannelConfig == FakeStereo) {
        for (u32 i = 0; i < numSamples; i++) {
            audioPtrR[i] *= -1.0f;
        }
    }

    if (currentChannelConfig == Mono) {
        // copy left channel into right channel if processing is in mono
        buffer.copyFrom(1, 0, buffer, 0, 0, (int)numSamples);
    }
    
    // FrameMark;
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
            DBG("could not load the stored ir file");
        }
    }
}

void Processor::initParameters() {
    for (size_t i = 0; i < N_PARAMS; i++) {
        parameterChanged(paramInfos[i].id.toString(), 
                         *apvts.getRawParameterValue(paramInfos[i].id));
    }
}

void Processor::parameterChanged(const juce::String &parameterId, float newValue) {
    ZoneScoped;

    const auto id = juce::Identifier(parameterId);

    if (id == paramInfos[GATE_THRESH].id 
        || id == paramInfos[GATE_RETURN].id) 
    {
        float thresh     = *apvts.getRawParameterValue(paramInfos[GATE_THRESH].id);
        float hysteresis = *apvts.getRawParameterValue(paramInfos[GATE_RETURN].id);
    
        noiseGate.threshold  = dbtoa(thresh);
        noiseGate.hysteresis = newValue;
        noiseGate.returnGain = dbtoa(thresh - hysteresis); 
            
        return;
    }

    if (id == paramInfos[INPUT_GAIN].id) {
        inputGain.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }

    
    if (id == paramInfos[GATE_ATTACK].id) {
        noiseGate.attackTimeMs = newValue;
        return;
    }
    
    if (id == paramInfos[GATE_RELEASE].id) {
        noiseGate.releaseTimeMs = newValue;
        return;
    }
    

    if (id == paramInfos[SCREAMER_AMOUNT].id || id == paramInfos[SCREAMER_FREQ].id) {

        float amount = *apvts.getRawParameterValue(paramInfos[SCREAMER_AMOUNT].id);        
        float freq = *apvts.getRawParameterValue(paramInfos[SCREAMER_FREQ].id);
    
        biteFilter.setCoefficients(freq, BOOST_BITE_Q, amount, samplerate);

        return;
    }

    if (id == paramInfos[TIGHT].id) {
        tightFilter.setCoefficients(newValue, samplerate);
        return;
    }

    if (id == paramInfos[INPUT_FILTER].id) {
        preamp.inputFilter.setCoefficients(newValue, preampSamplerate);
        return;
    }

    if (id == paramInfos[GATE_ACTIVE].id) {
        gateActive = (bool)newValue;
    }
    
    if (id == paramInfos[PREAMP_ACTIVE].id) {
        preampActive = (bool)newValue;
    }
    
    if (id == paramInfos[TONESTACK_ACTIVE].id) {
        tonestackActive = (bool)newValue;
    }
    
    if (id == paramInfos[EQ_ACTIVE].id) {
        EQActive = (bool)newValue;
    }
        
    if (id == paramInfos[IR_ACTIVE].id) {
        irLoader.active = (bool)newValue;
    }
    
    if (id == paramInfos[PREAMP_GAIN].id) {
        auto paramRange = apvts.getParameter(id)->getNormalisableRange();

        preamp.preGain.newTarget(tube_gain * scale(newValue, paramRange.start, paramRange.end, 0.0f, 1.0f, 2.5f),
                                  SMOOTH_PARAM_TIME, 
                                  preampSamplerate);
        preamp.brightCapFilter.setCoefficients(
            550.0, 
            scale_linear(newValue, paramRange.start, paramRange.end, -15.0f, 0.0f),
            preampSamplerate
        );
        
        return;
    }

    if (id == paramInfos[BRIGHT_CAP].id) {
        preamp.bright = (bool)newValue;
    }

    if (id == paramInfos[CHANNEL].id) {
        preamp.channel = (u8)newValue;
        return;
    }

    if (id == paramInfos[PREAMP_VOLUME].id) {
        preamp.postGain.newTarget(newValue + preamp.outputAttenuationdB, SMOOTH_PARAM_TIME, preampSamplerate);
        return;
    }
    
    if (id == paramInfos[STAGE0_LP].id
        || id == paramInfos[STAGE0_BYPASS].id
        || id == paramInfos[STAGE0_BIAS].id)
    {
        preamp.stage0LP.setCoefficients(
            *apvts.getRawParameterValue(paramInfos[STAGE0_LP].id), 
            preampSamplerate);
        
        preamp.cathodeBypassFilter0.setCoefficients(280.0, 
            *apvts.getRawParameterValue(paramInfos[STAGE0_BYPASS].id), 
            preampSamplerate);

        preamp.setBias(*apvts.getRawParameterValue(paramInfos[STAGE0_BIAS].id), 0);
        
        return;
    }
    
    if (id == paramInfos[STAGE1_LP].id
        || id == paramInfos[STAGE1_BYPASS].id
        || id == paramInfos[STAGE1_BIAS].id
        || id == paramInfos[STAGE1_ATTENUATION].id)
    {
        preamp.stage1LP.setCoefficients(
            *apvts.getRawParameterValue(paramInfos[STAGE1_LP].id), 
            preampSamplerate);
        
        preamp.cathodeBypassFilter1.setCoefficients(280.0, 
            *apvts.getRawParameterValue(paramInfos[STAGE1_BYPASS].id), 
            preampSamplerate);

        preamp.setBias(*apvts.getRawParameterValue(paramInfos[STAGE1_BIAS].id), 1);

        preamp.stage1Gain.newTarget(tube_gain * (*apvts.getRawParameterValue(paramInfos[STAGE1_ATTENUATION].id)), 
                                    SMOOTH_PARAM_TIME, preampSamplerate);

        return;
    }
    
    if (id == paramInfos[STAGE2_LP].id
        || id == paramInfos[STAGE2_BYPASS].id
        || id == paramInfos[STAGE2_BIAS].id
        || id == paramInfos[STAGE2_ATTENUATION].id)
    {
        preamp.stage2LP.setCoefficients(
            *apvts.getRawParameterValue(paramInfos[STAGE2_LP].id), 
            preampSamplerate);
        
        preamp.cathodeBypassFilter2.setCoefficients(280.0, 
            *apvts.getRawParameterValue(paramInfos[STAGE2_BYPASS].id), 
            preampSamplerate);

        preamp.setBias(*apvts.getRawParameterValue(paramInfos[STAGE2_BIAS].id), 2);

        preamp.stage2Gain.newTarget(tube_gain * (*apvts.getRawParameterValue(paramInfos[STAGE2_ATTENUATION].id)), 
                                    SMOOTH_PARAM_TIME, preampSamplerate);

        return;
    }
    
    if (id == paramInfos[STAGE3_LP].id
        || id == paramInfos[STAGE3_BYPASS].id
        || id == paramInfos[STAGE3_BIAS].id
        || id == paramInfos[STAGE3_ATTENUATION].id)
    {
        preamp.stage3LP.setCoefficients(
            *apvts.getRawParameterValue(paramInfos[STAGE3_LP].id), 
            preampSamplerate);
        
        preamp.cathodeBypassFilter3.setCoefficients(280.0, 
            *apvts.getRawParameterValue(paramInfos[STAGE3_BYPASS].id), 
            preampSamplerate);

        preamp.setBias(*apvts.getRawParameterValue(paramInfos[STAGE3_BIAS].id), 3);

        preamp.stage3Gain.newTarget(tube_gain * (*apvts.getRawParameterValue(paramInfos[STAGE3_ATTENUATION].id)), 
                                    SMOOTH_PARAM_TIME, preampSamplerate);

        return;
    }
    
    if (id == paramInfos[STAGE4_LP].id
        || id == paramInfos[STAGE4_BYPASS].id
        || id == paramInfos[STAGE4_BIAS].id)
    {
        preamp.stage4LP.setCoefficients(
            *apvts.getRawParameterValue(paramInfos[STAGE4_LP].id), 
            preampSamplerate);
        
        preamp.cathodeBypassFilter4.setCoefficients(280.0, 
            *apvts.getRawParameterValue(paramInfos[STAGE4_BYPASS].id), 
            preampSamplerate);

        preamp.setBias(*apvts.getRawParameterValue(paramInfos[STAGE4_BIAS].id), 4);
        return;
    }
    
    if (id == paramInfos[TONESTACK_BASS].id
        || id == paramInfos[TONESTACK_MIDDLE].id
        || id == paramInfos[TONESTACK_TREBBLE].id
        || id == paramInfos[TONESTACK_MODEL].id)
    {
        TonestackModel model = static_cast<TonestackModel>((int)*apvts.getRawParameterValue(paramInfos[TONESTACK_MODEL].id));
        
        if (toneStack.model != model) {
            toneStack.setModel(model);
        }
        
        float bassValue = (float)*apvts.getRawParameterValue(paramInfos[TONESTACK_BASS].id);
        float midValue = (float)*apvts.getRawParameterValue(paramInfos[TONESTACK_MIDDLE].id);
        float trebbleValue = (float)*apvts.getRawParameterValue(paramInfos[TONESTACK_TREBBLE].id);
            
        toneStack.bassParam.newTarget(bassValue/10.0f,       100.0f, samplerate);
        toneStack.midParam.newTarget(midValue/10.0f,         100.0f, samplerate);
        toneStack.trebbleParam.newTarget(trebbleValue/10.0f, 100.0f, samplerate);
        return;
    }

    if (id == paramInfos[RESONANCE].id) {
        resonanceFilter.setCoefficients(RESONANCE_FREQUENCY, scale_linear(newValue, 0.0f, 10.0f, 0.0f, 24.0f), samplerate);
        return;
    }

    if (id == paramInfos[PRESENCE].id) {
        presenceFilter.setCoefficients(PRESENCE_FREQUENCY, scale_linear(newValue, 0.0f, 10.0f, 0.0f, 18.0f), samplerate);
        return;
    }
    
    if (id == paramInfos[LOW_CUT_FREQ].id) {
        
        EQ.lowCut.setCoefficients(newValue, 0.7, 0.0, samplerate);
        return;
    }
    
    if (id == paramInfos[LOW_SHELF_FREQ].id
        || id == paramInfos[LOW_SHELF_GAIN].id)
    {
        
        EQ.lowShelf.setCoefficients(*apvts.getRawParameterValue(paramInfos[LOW_SHELF_FREQ].id), 0.7,
                                    *apvts.getRawParameterValue(paramInfos[LOW_SHELF_GAIN].id), samplerate);
        return;
    }
    
    if (id == paramInfos[LOWMID_FREQ].id
        || id == paramInfos[LOWMID_GAIN].id
        || id == paramInfos[LOWMID_Q].id)
    {
    
        EQ.lowMid.setCoefficients(*apvts.getRawParameterValue(paramInfos[LOWMID_FREQ].id),
                                  *apvts.getRawParameterValue(paramInfos[LOWMID_Q].id),
                                  *apvts.getRawParameterValue(paramInfos[LOWMID_GAIN].id), samplerate);
        return;
    }
    
    if (id == paramInfos[MID_FREQ].id
        || id == paramInfos[MID_GAIN].id
        || id == paramInfos[MID_Q].id)
    {
        EQ.mid.setCoefficients(*apvts.getRawParameterValue(paramInfos[MID_FREQ].id),
                               *apvts.getRawParameterValue(paramInfos[MID_Q].id),
                               *apvts.getRawParameterValue(paramInfos[MID_GAIN].id), samplerate);
        return;
    }
    
    if (id == paramInfos[HIGH_FREQ].id
        || id == paramInfos[HIGH_GAIN].id
        || id == paramInfos[HIGH_Q].id)
    {
        EQ.high.setCoefficients(*apvts.getRawParameterValue(paramInfos[HIGH_FREQ].id),
                                *apvts.getRawParameterValue(paramInfos[HIGH_Q].id),
                                *apvts.getRawParameterValue(paramInfos[HIGH_GAIN].id), samplerate);
        return;
    }
    
    if (id == paramInfos[HIGH_SHELF_FREQ].id
        || id == paramInfos[HIGH_SHELF_GAIN].id)
    {
        EQ.highShelf.setCoefficients(*apvts.getRawParameterValue(paramInfos[HIGH_SHELF_FREQ].id), 0.7,
                                     *apvts.getRawParameterValue(paramInfos[HIGH_SHELF_GAIN].id), samplerate);
        return;
    }
    
    if (id == paramInfos[HIGH_CUT_FREQ].id) {        
        EQ.highCut.setCoefficients(*apvts.getRawParameterValue(paramInfos[HIGH_CUT_FREQ].id), 0.7, 0.0, samplerate);
        return;
    }


    if (id == paramInfos[MASTER_VOLUME].id) {
        masterVolume.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }
    
    if (id == paramInfos[CHANNEL_CONFIG].id) {
        channelConfig = (u8)newValue;
        return;
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout Processor::createParameterLayout()
{   
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.reserve(N_PARAMS);
    
    auto attributes = juce::AudioParameterFloatAttributes()
                        .withStringFromValueFunction ([] (auto x, auto) { return juce::String (x, 2); });    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[INPUT_GAIN].id.toString(), "Input Gain", 
        juce::NormalisableRange<float>(-36.0f, 12.0f, 0.1f, 1.0f), paramInfos[INPUT_GAIN].defaultValue, attributes
    ));

    // Noise gate params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[GATE_THRESH].id.toString(), "Gate Thresh", 
        juce::NormalisableRange<float>(-96.0f, -40.0f, 0.1f, 1.0f), paramInfos[GATE_THRESH].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[GATE_ATTACK].id.toString(), "Attack Time", 
        juce::NormalisableRange<float>(0.1f, 15.0f, 0.1f, 1.0f), paramInfos[GATE_ATTACK].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[GATE_RELEASE].id.toString(), "Release Time", 
        juce::NormalisableRange<float>(1.0f, 25.0f, 0.1f, 1.0f), paramInfos[GATE_RELEASE].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[GATE_RETURN].id.toString(), "Hysteresis (not implemented)", 
        juce::NormalisableRange<float>(0.0f, 20.0f, 0.1f, 1.0f), paramInfos[GATE_RETURN].defaultValue, attributes
    ));
    
    // Input Boost params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[SCREAMER_AMOUNT].id.toString(), "Screamer", 
        juce::NormalisableRange<float>(0.0f, 30.0f, 0.1f, 1.0f), paramInfos[SCREAMER_AMOUNT].defaultValue, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[SCREAMER_FREQ].id.toString(), "Screamer Freq", 
        juce::NormalisableRange<float>(500.0f, 4000.0f, 1.0f, 0.7f), paramInfos[SCREAMER_FREQ].defaultValue, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[TIGHT].id.toString(), "Tight", 
        juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.7f), paramInfos[TIGHT].defaultValue, attributes
    ));

        
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        paramInfos[GATE_ACTIVE].id.toString(), "Activate Gate",
        (bool)paramInfos[GATE_ACTIVE].defaultValue
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        paramInfos[PREAMP_ACTIVE].id.toString(), "Activate Preamp",
        (bool)paramInfos[PREAMP_ACTIVE].defaultValue
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        paramInfos[TONESTACK_ACTIVE].id.toString(), "Activate Tonestack",
        (bool)paramInfos[TONESTACK_ACTIVE].defaultValue
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        paramInfos[EQ_ACTIVE].id.toString(), "Activate EQ",
        (bool)paramInfos[EQ_ACTIVE].defaultValue
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        paramInfos[IR_ACTIVE].id.toString(), "Activate IR Loader", 
        (bool)paramInfos[IR_ACTIVE].defaultValue
    ));


    // Preamp Params 
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[PREAMP_GAIN].id.toString(), "Pre Gain",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), paramInfos[PREAMP_GAIN].defaultValue, attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        paramInfos[BRIGHT_CAP].id.toString(), "Bright Switch", 
        (bool)paramInfos[BRIGHT_CAP].defaultValue
    ));


    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[INPUT_FILTER].id.toString(), "Input Filter", 
        juce::NormalisableRange<float>(0.0f, 1000.0f, 1.0f, 0.7f), paramInfos[INPUT_FILTER].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        paramInfos[CHANNEL].id.toString(), "Amp Channel", 1, 4, (int)paramInfos[CHANNEL].defaultValue
    ));
    
    // Tubes params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE0_LP].id.toString(), "STAGE0_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), paramInfos[STAGE0_LP].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE0_BYPASS].id.toString(), "STAGE0_BYPASS", 
        juce::NormalisableRange<float>(-15.0f, 0.0f, 0.1f, 1.0f),  paramInfos[STAGE0_BYPASS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE0_BIAS].id.toString(), "STAGE0_BIAS", 
        juce::NormalisableRange<float>(0.0, 1.0f, 0.01f, 1.0f), paramInfos[STAGE0_BIAS].defaultValue, attributes
    ));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE1_LP].id.toString(), "STAGE1_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), paramInfos[STAGE1_LP].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE1_BYPASS].id.toString(), "STAGE1_BYPASS", 
        juce::NormalisableRange<float>(-15.0f, 0.0f, 0.1f, 1.0f), paramInfos[STAGE1_BYPASS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE1_BIAS].id.toString(), "STAGE1_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), paramInfos[STAGE1_BIAS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE1_ATTENUATION].id.toString(), "STAGE1_ATTENUATION", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.7f), paramInfos[STAGE1_ATTENUATION].defaultValue, attributes
    ));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE2_LP].id.toString(), "STAGE2_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), paramInfos[STAGE2_LP].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE2_BYPASS].id.toString(), "STAGE2_BYPASS", 
        juce::NormalisableRange<float>(-15.0f, 0.0f, 0.1f, 1.0f),  paramInfos[STAGE2_BYPASS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE2_BIAS].id.toString(), "STAGE2_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), paramInfos[STAGE2_BIAS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE2_ATTENUATION].id.toString(), "STAGE2_ATTENUATION", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.7f), paramInfos[STAGE2_ATTENUATION].defaultValue, attributes
    ));
    
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE3_LP].id.toString(), "STAGE3_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0f, 0.7f), paramInfos[STAGE3_LP].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE3_BYPASS].id.toString(), "STAGE3_BYPASS", 
        juce::NormalisableRange<float>(-15.0f, 0.0f, 0.1f, 1.0f),  paramInfos[STAGE3_BYPASS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE3_BIAS].id.toString(), "STAGE3_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), paramInfos[STAGE3_BIAS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE3_ATTENUATION].id.toString(), "STAGE3_ATTENUATION", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 0.7f), paramInfos[STAGE3_ATTENUATION].defaultValue, attributes
    ));
    
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE4_LP].id.toString(), "STAGE4_LP", 
        juce::NormalisableRange<float>(1000.0f, 20000.0f, 1.0, 0.7f), paramInfos[STAGE4_LP].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE4_BYPASS].id.toString(), "STAGE4_BYPASS", 
        juce::NormalisableRange<float>(-15.0f, 0.0f, 0.1f, 1.0f), paramInfos[STAGE4_BYPASS].defaultValue, attributes
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[STAGE4_BIAS].id.toString(), "STAGE4_BIAS", 
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f), paramInfos[STAGE4_BIAS].defaultValue, attributes
    ));
    
    
    // Tonestack params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[TONESTACK_BASS].id.toString(), "Bass", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), paramInfos[TONESTACK_BASS].defaultValue, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[TONESTACK_MIDDLE].id.toString(), "Mid", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), paramInfos[TONESTACK_MIDDLE].defaultValue, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[TONESTACK_TREBBLE].id.toString(), "Trebble", 
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.01f, 1.0f), paramInfos[TONESTACK_TREBBLE].defaultValue, attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        paramInfos[TONESTACK_MODEL].id.toString(), "Tonestack model", 
        juce::StringArray{"SLO", "Savage", "JCM"}, (int)paramInfos[TONESTACK_MODEL].defaultValue
    ));


    // Post Preamp params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[PREAMP_VOLUME].id.toString(), "Post Gain", 
        juce::NormalisableRange<float>(-36.0f, 36.0f, 0.1f, 1.0f), paramInfos[PREAMP_VOLUME].defaultValue, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[RESONANCE].id.toString(), "Resonance",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), paramInfos[RESONANCE].defaultValue, attributes
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[PRESENCE].id.toString(), "Presence",
        juce::NormalisableRange<float>(0.0f, 10.0f, 0.1f, 1.0f), paramInfos[PRESENCE].defaultValue, attributes
    ));

    // EQ params
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[LOW_CUT_FREQ].id.toString(), "Low cut",
        juce::NormalisableRange<float>(10.0f, 200.0f, 0.1f, 0.7f), paramInfos[LOW_CUT_FREQ].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[LOW_SHELF_FREQ].id.toString(), "Low Shelf Freq",
        juce::NormalisableRange<float>(10.0f, 250.0f, 0.1f, 0.7f), paramInfos[LOW_SHELF_FREQ].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[LOW_SHELF_GAIN].id.toString(), "Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), paramInfos[LOW_SHELF_GAIN].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[LOWMID_FREQ].id.toString(), "Freq",
        juce::NormalisableRange<float>(100.0f, 8000.0f, 0.1f, 0.7f), paramInfos[LOWMID_FREQ].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[LOWMID_GAIN].id.toString(), "Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), paramInfos[LOWMID_GAIN].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[LOWMID_Q].id.toString(), "Q",
        juce::NormalisableRange<float>(0.1f, 4.0f, 0.01f, 0.5f), paramInfos[LOWMID_Q].defaultValue
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[MID_FREQ].id.toString(), "Freq",
        juce::NormalisableRange<float>(100.0f, 8000.0f, 1.0f, 0.7f), paramInfos[MID_FREQ].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[MID_GAIN].id.toString(), "Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), paramInfos[MID_GAIN].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[MID_Q].id.toString(), "Q",
        juce::NormalisableRange<float>(0.1f, 4.0f, 0.01f, 0.5f), paramInfos[MID_Q].defaultValue
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[HIGH_FREQ].id.toString(), "Freq",
        juce::NormalisableRange<float>(100.0f, 8000.0f, 1.0f, 0.7f), paramInfos[HIGH_FREQ].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[HIGH_GAIN].id.toString(), "Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), paramInfos[HIGH_GAIN].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[HIGH_Q].id.toString(), "Q",
        juce::NormalisableRange<float>(0.1f, 4.0f, 0.01f, 0.5f), paramInfos[HIGH_Q].defaultValue
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[HIGH_SHELF_FREQ].id.toString(), "High Shelf Freq",
        juce::NormalisableRange<float>(4000.0f, 20000.0f, 1.0f, 0.7f), paramInfos[HIGH_SHELF_FREQ].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[HIGH_SHELF_GAIN].id.toString(), "Gain",
        juce::NormalisableRange<float>(-18.0f, 18.0f, 0.1f, 1.0f), paramInfos[HIGH_SHELF_GAIN].defaultValue, attributes
    ));
        
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[HIGH_CUT_FREQ].id.toString(), "High cut",
        juce::NormalisableRange<float>(4000.0f, 20000.0f, 1.0f, 0.7f), paramInfos[HIGH_CUT_FREQ].defaultValue, attributes
    ));


    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        paramInfos[MASTER_VOLUME].id.toString(), "Master Vol",
        juce::NormalisableRange<float>(-18.0f, 12.0f, 0.1f, 1.0f), paramInfos[MASTER_VOLUME].defaultValue, attributes
    ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        paramInfos[CHANNEL_CONFIG].id.toString(), "Channel config",
        juce::StringArray{"Mono","Fake Stereo", "Stereo"}, (int)paramInfos[CHANNEL_CONFIG].defaultValue
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}
