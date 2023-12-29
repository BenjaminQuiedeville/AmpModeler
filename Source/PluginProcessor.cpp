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
    preBoost  = new Boost();
    preamp    = new PreampDistorsion();
    toneStack = new Tonestack();
    irLoader  = new IRLoader();

    resonanceFilter = new Biquad(FilterType::BIQUAD_LOWSHELF);
    presenceFilter  = new Biquad(FilterType::BIQUAD_HIGHSHELF);

    masterVolume = new SmoothParamLinear();

    testOsc = new TestOsc();

    for (uint8_t i = 0; i < N_PARAMS; i++) {
        apvts->addParameterListener(ParamIDs[i], this);
    }
}

AmpModelerAudioProcessor::~AmpModelerAudioProcessor() {

    for (uint8_t i = 0; i < N_PARAMS; i++) {
        apvts->removeParameterListener(ParamIDs[i], this);
    }

    delete noiseGate;
    delete preBoost;
    delete preamp;
    delete toneStack;
    delete irLoader;

    delete resonanceFilter;
    delete presenceFilter;

    delete masterVolume;

    delete testOsc;

    if (intputSignalCopy) {
        free(intputSignalCopy);
    }

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
    index;
    return;
}

const juce::String AmpModelerAudioProcessor::getProgramName (int index)
{
    index;
    return {};
}

void AmpModelerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    newName; index;
    return;
}

//==============================================================================
void AmpModelerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    samplerate = sampleRate;

    noiseGate->prepareToPlay(sampleRate);
    preBoost->prepareToPlay(sampleRate);

    preamp->prepareToPlay(sampleRate, samplesPerBlock);

    masterVolume->init(1.0);
    masterVolume->newTarget(DB_TO_GAIN(-6.0f), SMOOTH_PARAM_TIME, samplerate);

    toneStack->prepareToPlay(sampleRate);
    irLoader->init(sampleRate, samplesPerBlock);

    resonanceFilter->setCoefficients(RESONANCE_FREQUENCY, RESONANCE_Q, 0.0, sampleRate);

    presenceFilter->setCoefficients(PRESENCE_FREQUENCY, PRESENCE_Q, 0.0, sampleRate);

    if (!intputSignalCopy) {
        intputSignalCopy = (sample_t *)malloc(samplesPerBlock * sizeof(sample_t));
    }

    testOsc->setFreq(100.0, sampleRate);

    initParameters();
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
    midiMessages;

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    size_t numSamples = (size_t)buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, (int)numSamples);
    }


    sample_t *audioPtr = buffer.getWritePointer(0);

    if (doTestOsc) {
        for (size_t i = 0; i < numSamples; i++) {
            audioPtr[i] = testOsc->generateNextSample();
        }
    }


    memcpy(intputSignalCopy, audioPtr, numSamples);

    /******PROCESS********/
    // preBoost->process(audioPtr, numSamples);
    preamp->process(audioPtr, numSamples);
    // noiseGate->process(audioPtr, intputSignalCopy, numSamples);
    toneStack->process(audioPtr, numSamples);
    irLoader->process(audioPtr, numSamples);
    
    for (size_t i = 0; i < numSamples; i++) {
        audioPtr[i] *= (sample_t)DB_TO_GAIN(masterVolume->nextValue());
    }

    // copy left channel into right channel
    buffer.copyFrom(1, 0, buffer, 0, 0, (int)numSamples);

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

    noiseGate->threshold = *apvts->getRawParameterValue(ParamIDs[GATE_THRESH]);

    preBoost->level->newTarget(*apvts->getRawParameterValue(ParamIDs[PRE_BOOST]), 
                               SMOOTH_PARAM_TIME, samplerate);
    preBoost->tightFilter->setCoefficients(*apvts->getRawParameterValue(ParamIDs[TIGHT]), samplerate);
    preBoost->biteFilter->setCoefficients(BOOST_BITE_FREQ, BOOST_BITE_Q, 
                                          *apvts->getRawParameterValue(ParamIDs[BITE]), 
                                          samplerate);

    preamp->preGain->newTarget(*apvts->getRawParameterValue(ParamIDs[PREAMP_GAIN]), 
                               SMOOTH_PARAM_TIME, samplerate * preamp->upSampleFactor);

    float bassEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_BASS]);
    float trebbleEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_TREBBLE]);
    float midEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_MIDDLE]);
    toneStack->updateCoefficients(trebbleEQgain, midEQgain, bassEQgain);

    preamp->postGain->newTarget(*apvts->getRawParameterValue(ParamIDs[PREAMP_VOLUME]), 
                                SMOOTH_PARAM_TIME, samplerate);

    resonanceFilter->setCoefficients(RESONANCE_FREQUENCY, RESONANCE_Q, 
                                     *apvts->getRawParameterValue(ParamIDs[RESONANCE]), 
                                     samplerate);
    presenceFilter->setCoefficients(PRESENCE_FREQUENCY, PRESENCE_Q, 
                                    *apvts->getRawParameterValue(ParamIDs[PRESENCE]), 
                                    samplerate);
    
    masterVolume->newTarget(*apvts->getRawParameterValue(ParamIDs[MASTER_VOLUME]), 
                            SMOOTH_PARAM_TIME, samplerate);
}

void AmpModelerAudioProcessor::parameterChanged(const juce::String &parameterID, float newValue) {

    if (parameterID == ParamIDs[GATE_THRESH]) {
        noiseGate->threshold = newValue;
        return;
    }

    if (parameterID == ParamIDs[BITE]) {
        preBoost->biteFilter->setCoefficients(BOOST_BITE_FREQ, BOOST_BITE_Q, newValue, samplerate);

        return;
    }

    if (parameterID == ParamIDs[TIGHT]) {
        preBoost->tightFilter->setCoefficients(newValue, samplerate);
        return;
    }

    if (parameterID == ParamIDs[PRE_BOOST]) {
        preBoost->level->newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }

    if (parameterID == ParamIDs[INPUT_FILTER]) {
        preamp->inputFilter->setCoefficients(newValue, samplerate*(preamp->upSampleFactor));
        return;
    }

    if (parameterID == ParamIDs[PREAMP_GAIN]) {
        preamp->preGain->newTarget(newValue, SMOOTH_PARAM_TIME, samplerate * preamp->upSampleFactor);
        return;
    }

    if (parameterID == ParamIDs[PREAMP_VOLUME]) {
        preamp->postGain->newTarget(newValue, SMOOTH_PARAM_TIME, samplerate * preamp->upSampleFactor);
        return;
    }

    if (parameterID == ParamIDs[TONESTACK_BASS]
        || parameterID == ParamIDs[TONESTACK_MIDDLE]
        || parameterID == ParamIDs[TONESTACK_TREBBLE])
    {
        float bassEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_BASS]);
        float trebbleEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_TREBBLE]);
        float midEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_MIDDLE]);
        toneStack->updateCoefficients(trebbleEQgain, midEQgain, bassEQgain);
        return;
    }

    if (parameterID == ParamIDs[RESONANCE]) {
        resonanceFilter->setCoefficients(RESONANCE_FREQUENCY, RESONANCE_Q, newValue, samplerate);
        return;
    }

    if (parameterID == ParamIDs[PRESENCE]) {
        presenceFilter->setCoefficients(PRESENCE_FREQUENCY, PRESENCE_Q, newValue, samplerate);
        return;
    }

    if (parameterID == ParamIDs[MASTER_VOLUME]) {
        masterVolume->newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }

}

juce::AudioProcessorValueTreeState::ParameterLayout AmpModelerAudioProcessor::createParameterLayout()
{   

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_THRESH], "Gate Thresh", -80.0f, -40.0f, -75.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[BITE], "Bite", 0.0f, 20.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TIGHT], "Tight", 20.0f, 1000.0f, 0.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PRE_BOOST], "Boost", 0.0f, 20.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_GAIN], "Pre Gain", 0.0f, 10.0f, 0.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[INPUT_FILTER], "Input Filter", 0.0f, 700.0f, 40.0f
    ));
    

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_BASS], "Bass", 0.0f, 1.0f, 0.5f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_MIDDLE], "Mid", 0.0f, 1.0f, 0.5f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_TREBBLE], "Trebble", 0.0f, 1.0f, 0.5f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_VOLUME], "Post Gain", -36.0f, 12.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[RESONANCE], "Reson", 0.0f, 6.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PRESENCE], "Presence", 0.0f, 6.0f, 0.0f
    ));


    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[MASTER_VOLUME], "Master Vol", -20.0f, 0.0f, -6.0f
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpModelerAudioProcessor();
}
