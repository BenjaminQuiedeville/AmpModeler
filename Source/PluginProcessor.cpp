#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "JucePluginDefines.h"

//==============================================================================
AmpModelerAudioProcessor::AmpModelerAudioProcessor()
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
{

    apvts =  new juce::AudioProcessorValueTreeState(*this, nullptr, "Params", createParameterLayout());
    
    noiseGate = new NoiseGate();
    preBoost  = new PreBoost();
    preamp    = new PreampDistorsion();
    postEQ    = new ThreeBandEQ();
    irLoader  = new IRLoader();

    for (uint8_t i = 0; i < N_PARAMS; i++) {
        apvts->addParameterListener(ParamsID[i], this);
    }
}

AmpModelerAudioProcessor::~AmpModelerAudioProcessor() {

    for (uint8_t i = 0; i < N_PARAMS; i++) {
        apvts->removeParameterListener(ParamsID[i], this);
    }

    delete noiseGate;
    delete preBoost;
    delete preamp;
    delete postEQ;
    delete irLoader;

    delete apvts;
}

//==============================================================================
const juce::String AmpModelerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AmpModelerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AmpModelerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AmpModelerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AmpModelerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AmpModelerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AmpModelerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AmpModelerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AmpModelerAudioProcessor::getProgramName (int index)
{
    return {};
}

void AmpModelerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AmpModelerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    noiseGate->prepareToPlay(sampleRate);

    preamp->prepareToPlay(sampleRate, samplesPerBlock);

    masterVolume.init(sampleRate, 0.02, 1.0, SMOOTH_PARAM_LIN);
    masterVolume.newTarget(DB_TO_GAIN(-6.0f));

    postEQ->prepareToPlay(sampleRate);
    irLoader->init(sampleRate, samplesPerBlock);
}

void AmpModelerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AmpModelerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AmpModelerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    size_t numSamples = (size_t)buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, numSamples);
    }


    float *audioPtr = buffer.getWritePointer(0);

    /******PROCESS********/
    noiseGate->process(audioPtr, numSamples);

    preamp->process(audioPtr, numSamples);
    postEQ->process(audioPtr, numSamples);
    irLoader->process(audioPtr, numSamples);
    
    for (size_t i = 0; i < numSamples; i++) {
        audioPtr[i] *= masterVolume.nextValue();
    }

    // copy left channel into right channel
    buffer.copyFrom(1, 0, buffer, 0, 0, numSamples);

}

//==============================================================================
bool AmpModelerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AmpModelerAudioProcessor::createEditor()
{
    return new AmpModelerAudioProcessorEditor (*this);
}

//==============================================================================
void AmpModelerAudioProcessor::getStateInformation (juce::MemoryBlock& destData) {
    // save params
    juce::MemoryOutputStream stream(destData, false);
    apvts->state.writeToStream(stream);
}   

void AmpModelerAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // Recall params
    juce::ValueTree importedTree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));

    if (importedTree.isValid()) {
        apvts->state = importedTree;
        initParameters();
    }

}

void AmpModelerAudioProcessor::initParameters() {

    noiseGate->threshold = *apvts->getRawParameterValue(ParamsID[GATE_THRESH]);

    preamp->preGain.newTarget(*apvts->getRawParameterValue(ParamsID[PREAMP_GAIN]));

    float bassEQgain = *apvts->getRawParameterValue(ParamsID[TONESTACK_BASS]);
    float trebbleEQgian = *apvts->getRawParameterValue(ParamsID[TONESTACK_TREBBLE]);
    float midEQgian = *apvts->getRawParameterValue(ParamsID[TONESTACK_MIDDLE]);
    postEQ->updateGains(bassEQgain, midEQgian, trebbleEQgian);

    float preampVolume = *apvts->getRawParameterValue(ParamsID[PREAMP_VOLUME]);
    preamp->postGain.newTarget(preampVolume);
    
    masterVolume.newTarget(*apvts->getRawParameterValue(ParamsID[MASTER_VOLUME]));
}

void AmpModelerAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue) {

    if (parameterID == ParamsID[GATE_THRESH]) {
        noiseGate->threshold = *apvts->getRawParameterValue(ParamsID[GATE_THRESH]);
        return;
    }

    if (parameterID == ParamsID[BITE]) {

        return;
    }

    if (parameterID == ParamsID[TIGHT]) {

        return;
    }

    if (parameterID == ParamsID[PRE_BOOST]) {

        return;
    }

    if (parameterID == ParamsID[PREAMP_GAIN]) {
        preamp->preGain.newTarget(*apvts->getRawParameterValue(ParamsID[PREAMP_GAIN]));
        return;
    }

    if (parameterID == ParamsID[TONESTACK_BASS]
        || parameterID == ParamsID[TONESTACK_MIDDLE]
        || parameterID == ParamsID[TONESTACK_TREBBLE])
    {
        float bassEQgain = *apvts->getRawParameterValue(ParamsID[TONESTACK_BASS]);
        float trebbleEQgian = *apvts->getRawParameterValue(ParamsID[TONESTACK_TREBBLE]);
        float midEQgian = *apvts->getRawParameterValue(ParamsID[TONESTACK_MIDDLE]);
        postEQ->updateGains(bassEQgain, midEQgian, trebbleEQgian);
        return;
    }

    if (parameterID == ParamsID[PREAMP_VOLUME]) {
        float preampVolume = *apvts->getRawParameterValue(ParamsID[PREAMP_VOLUME]);
        preamp->postGain.newTarget(preampVolume);
        return;
    }

    if (parameterID == ParamsID[RESONANCE]) {

        return;
    }

    if (parameterID == ParamsID[PRESENCE]) {

        return;
    }

    if (parameterID == ParamsID[MASTER_VOLUME]) {
        masterVolume.newTarget(DB_TO_GAIN(*apvts->getRawParameterValue(ParamsID[MASTER_VOLUME])));
        return;
    }

}

juce::AudioProcessorValueTreeState::ParameterLayout AmpModelerAudioProcessor::createParameterLayout()
{   

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[GATE_THRESH], "Gate Thresh", -80.0f, -40.0f, -70.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[BITE], "Bite", 0.0f, 10.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[TIGHT], "Tight", 0.0f, 10.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[PRE_BOOST], "Boost", 0.0f, 20.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[PREAMP_GAIN], "Pre Gain", -40.0f, 40.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[TONESTACK_BASS], "Bass", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[TONESTACK_MIDDLE], "Mid", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[TONESTACK_TREBBLE], "Trebble", -12.0f, 12.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[PREAMP_VOLUME], "Post Gain", -30.0f, 10.0f, -12.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[RESONANCE], "Reson", 0.0f, 3.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[PRESENCE], "Presence", 0.0f, 3.0f, 0.0f));


    params.push_back(std::make_unique<juce::AudioParameterFloat>(ParamsID[MASTER_VOLUME], "Master Vol", -20.0f, 0.0f, -6.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpModelerAudioProcessor();
}
