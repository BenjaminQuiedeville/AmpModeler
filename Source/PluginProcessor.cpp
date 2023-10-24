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
                       ), 
        apvts(*this, nullptr, "Params", createParameterLayout())
#endif
{

    noiseGate = new NoiseGate();
    preBoost  = new PreBoost();
    preamp    = new PreampDistorsion();
    postEQ    = new ThreeBandEQ();
    irLoader  = new IRLoader();

    apvts.addParameterListener("GATE_THRESH", this);
    apvts.addParameterListener("BITE", this);
    apvts.addParameterListener("TIGHT", this);
    apvts.addParameterListener("PRE_BOOST", this);
    apvts.addParameterListener("PREAMP_GAIN", this);
    apvts.addParameterListener("3_BAND_EQ_BASS", this);
    apvts.addParameterListener("3_BAND_EQ_MIDDLE", this);
    apvts.addParameterListener("3_BAND_EQ_TREBBLE", this);
    apvts.addParameterListener("PREAMP_VOLUME", this);
    apvts.addParameterListener("RESONANCE", this);
    apvts.addParameterListener("PRESENCE", this);
    apvts.addParameterListener("MASTER_VOLUME", this);
}

AmpModelerAudioProcessor::~AmpModelerAudioProcessor() {

    apvts.removeParameterListener("GATE_THRESH", this);
    apvts.removeParameterListener("BITE", this);
    apvts.removeParameterListener("TIGHT", this);
    apvts.removeParameterListener("PRE_BOOST", this);
    apvts.removeParameterListener("PREAMP_GAIN", this);
    apvts.removeParameterListener("3_BAND_EQ_BASS", this);
    apvts.removeParameterListener("3_BAND_EQ_MIDDLE", this);
    apvts.removeParameterListener("3_BAND_EQ_TREBBLE", this);
    apvts.removeParameterListener("PREAMP_VOLUME", this);
    apvts.removeParameterListener("RESONANCE", this);
    apvts.removeParameterListener("PRESENCE", this);
    apvts.removeParameterListener("MASTER_VOLUME", this);

    delete noiseGate;
    delete preBoost;
    delete preamp;
    delete postEQ;
    delete irLoader;
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
    apvts.state.writeToStream(stream);
}   

void AmpModelerAudioProcessor::setStateInformation (const void* data, int sizeInBytes) {
    // Recall params
    juce::ValueTree importedTree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));

    if (importedTree.isValid()) {
        apvts.state = importedTree;
        initParameters();
    }

}

void AmpModelerAudioProcessor::initParameters() {

    noiseGate->threshold = *apvts.getRawParameterValue("GATE_THRESH");

    preamp->preGain.newTarget(*apvts.getRawParameterValue("PREAMP_GAIN"));

    float bassEQgain = *apvts.getRawParameterValue("3_BAND_EQ_BASS");
    float trebbleEQgian = *apvts.getRawParameterValue("3_BAND_EQ_TREBBLE");
    float midEQgian = *apvts.getRawParameterValue("3_BAND_EQ_MIDDLE");
    postEQ->updateGains(bassEQgain, midEQgian, trebbleEQgian);

    float preampVolume = *apvts.getRawParameterValue("PREAMP_VOLUME");
    preamp->postGain.newTarget(preampVolume);
    
    masterVolume.newTarget(*apvts.getRawParameterValue("MASTER_VOLUME"));
}

void AmpModelerAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue) {

    if (parameterID == "GATE_THRESH") {
        noiseGate->threshold = *apvts.getRawParameterValue("GATE_THRESH");
    }

    if (parameterID == "BITE") {

    }

    if (parameterID == "TIGHT") {

    }

    if (parameterID == "PRE_BOOST") {

    }

    if (parameterID == "PREAMP_GAIN") {
        preamp->preGain.newTarget(*apvts.getRawParameterValue("PREAMP_GAIN"));
    }

    if (parameterID == "3_BAND_EQ_BASS"
        || parameterID == "3_BAND_EQ_MIDDLE"
        || parameterID == "3_BAND_EQ_TREBBLE")
    {
        float bassEQgain = *apvts.getRawParameterValue("3_BAND_EQ_BASS");
        float trebbleEQgian = *apvts.getRawParameterValue("3_BAND_EQ_TREBBLE");
        float midEQgian = *apvts.getRawParameterValue("3_BAND_EQ_MIDDLE");
        postEQ->updateGains(bassEQgain, midEQgian, trebbleEQgian);
    }

    if (parameterID == "PREAMP_VOLUME") {
        float preampVolume = *apvts.getRawParameterValue("PREAMP_VOLUME");
        preamp->postGain.newTarget(preampVolume);
    }

    if (parameterID == "RESONANCE") {

    }

    if (parameterID == "PRESENCE") {

    }

    if (parameterID == "MASTER_VOLUME") {
        masterVolume.newTarget(DB_TO_GAIN(*apvts.getRawParameterValue("MASTER_VOLUME")));
    }


}

juce::AudioProcessorValueTreeState::ParameterLayout AmpModelerAudioProcessor::createParameterLayout()
{   

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("GATE_THRESH", "Gate Thresh", -80.0f, -40.0f, -70.0f));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>("BITE", "Bite", 0.0f, 10.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("TIGHT", "Tight", 0.0f, 10.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("PRE_BOOST", "Boost", 0.0f, 20.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PREAMP_GAIN", "Pre Gain", -40.0f, 40.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("3_BAND_EQ_BASS", "Bass", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("3_BAND_EQ_MIDDLE", "Mid", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("3_BAND_EQ_TREBBLE", "Trebble", -12.0f, 12.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("PREAMP_VOLUME", "Post Gain", -30.0f, 10.0f, -12.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RESONANCE", "Reson", 0.0f, 3.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("PRESENCE", "Presence", 0.0f, 3.0f, 0.0f));


    params.push_back(std::make_unique<juce::AudioParameterFloat>("MASTER_VOLUME", "Master Vol", -20.0f, 0.0f, -6.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpModelerAudioProcessor();
}
