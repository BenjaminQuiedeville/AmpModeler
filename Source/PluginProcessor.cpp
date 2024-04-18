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
// valueTree("IR paths", {}, 
//     {{"Group", 
//     {{"name", "IR names"}}, 
//     {{"Parameter", {{"id", "IR1"}, {"value", "C:/"}}}}}})
   
    
    noiseGate = new NoiseGate();
    preBoost  = new Boost();
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
    delete preBoost;
    delete preamp;
    delete toneStack;
    delete irLoader;

    if (intputSignalCopy) {
        free(intputSignalCopy);
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

    noiseGate->prepareToPlay(sampleRate);
    preBoost->prepareToPlay();

    preamp->prepareToPlay(sampleRate, samplesPerBlock);

    masterVolume.init(0.0);

    toneStack->prepareToPlay(sampleRate);
    irLoader->init(sampleRate, samplesPerBlock);

    if (!intputSignalCopy) {
        intputSignalCopy = (sample_t *)calloc(samplesPerBlock,  sizeof(sample_t));
    }

    testOsc.setFreq(200.0, sampleRate);

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

    size_t numSamples = (size_t)buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, (int)numSamples);
    }


    sample_t *audioPtr = buffer.getWritePointer(0);

    inputNoiseFilter.processBuffer(audioPtr, numSamples);


    if (doTestOsc) {
        for (size_t i = 0; i < numSamples; i++) {
            audioPtr[i] = testOsc.generateNextSample();
        }
    }

    memcpy(intputSignalCopy, audioPtr, numSamples * sizeof(sample_t));

    /******PROCESS********/
    preBoost->process(audioPtr, numSamples);
    preamp->process(audioPtr, numSamples);
    noiseGate->process(audioPtr, intputSignalCopy, numSamples);
    toneStack->process(audioPtr, numSamples);
    
    resonanceFilter.processBuffer(audioPtr, numSamples);
    presenceFilter.processBuffer(audioPtr, numSamples);
    
    irLoader->process(audioPtr, numSamples);
    
    for (size_t i = 0; i < numSamples; i++) {
        audioPtr[i] *= (sample_t)DB_TO_GAIN(masterVolume.nextValue());
    }

    // copy left channel into right channel
    buffer.copyFrom(1, 0, buffer, 0, 0, (int)numSamples);

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

//==============================================================================
void Processor::getStateInformation (juce::MemoryBlock& destData) {
    // save params
    apvts.state.appendChild(valueTree, nullptr); // ?????
    
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
        juce::File openedFile(irPath);
        
        if (openedFile.getFileExtension() == ".wav"){
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

    if (id == ParamIDs[GATE_THRESH]) {
        noiseGate->threshold = DB_TO_GAIN(newValue);
        return;
    }

    if (id == ParamIDs[BITE] || id == ParamIDs[BITE_FREQ]) {

        float amount = *apvts.getRawParameterValue(ParamIDs[BITE]);        
        float freq = *apvts.getRawParameterValue(ParamIDs[BITE_FREQ]);
    
        preBoost->biteFilter.setCoefficients(freq, BOOST_BITE_Q, amount, samplerate);

        return;
    }

    if (id == ParamIDs[TIGHT]) {
        preBoost->tightFilter.setCoefficients(newValue, samplerate);
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
            750.0, 0.4, 
            scale(newValue, paramRange.start, paramRange.end, -18.0f, 0.0f, 1.0f),
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
        ToneStackModel model = static_cast<ToneStackModel>((int)*apvts.getRawParameterValue(ParamIDs[TONESTACK_MODEL]));
        toneStack->comp->setModel(model);
        
        float bassParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_BASS]);
        float trebbleParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_TREBBLE]);
        float midParam = *apvts.getRawParameterValue(ParamIDs[TONESTACK_MIDDLE]);
        toneStack->updateCoefficients(trebbleParam/10.0f, midParam/10.0f, bassParam/10.0f, samplerate);
        return;
    }

    if (id == ParamIDs[RESONANCE]) {
        resonanceFilter.setCoefficients(RESONANCE_FREQUENCY, RESONANCE_Q, newValue, samplerate);
        return;
    }

    if (id == ParamIDs[PRESENCE]) {
        presenceFilter.setCoefficients(PRESENCE_FREQUENCY, PRESENCE_Q, newValue, samplerate);
        return;
    }

    if (id == ParamIDs[MASTER_VOLUME]) {
        masterVolume.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }

}

juce::AudioProcessorValueTreeState::ParameterLayout Processor::createParameterLayout()
{   

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_THRESH].toString(), "Gate Thresh", -90.0f, -40.0f, -75.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[BITE].toString(), "Bite", 0.0f, 30.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[BITE_FREQ].toString(), "Bite Freq", 500.0f, 3500.0f, 1700.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TIGHT].toString(), "Tight", 10.0f, 750.0f, 10.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_GAIN].toString(), "Pre Gain", 0.0f, 10.0f, 5.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[INPUT_FILTER].toString(), "Input Filter", 0.0f, 700.0f, 300.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        ParamIDs[CHANNEL].toString(), "Amp Channel", 1, 4, 1
    ));
    

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_BASS].toString(), "Bass", 0.0f, 10.0f, 5.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_MIDDLE].toString(), "Mid", 0.0f, 10.0f, 5.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TONESTACK_TREBBLE].toString(), "Trebble", 0.0f, 10.0f, 5.0f
    ));
    
    // params.push_back(std::make_unique<juce::AudioParameterInt>(
    //     ParamIDs[TONESTACK_MODEL].toString(), "Tonestack model", 1, 5, 1
    // ));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        ParamIDs[TONESTACK_MODEL].toString(), "Tonestack model", 
        juce::StringArray{"Savage", "JCM", "SLO", "Rect", "Orange"}, 0
    ));


    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_VOLUME].toString(), "Post Gain", -18.0f, 18.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[RESONANCE].toString(), "Reson", 0.0f, 12.0f, 6.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PRESENCE].toString(), "Presence", 0.0f, 12.0f, 6.0f
    ));


    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[MASTER_VOLUME].toString(), "Master Vol", -20.0f, 0.0f, -3.0f
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Processor();
}
