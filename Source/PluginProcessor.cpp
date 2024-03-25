/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
{

    apvts =  new juce::AudioProcessorValueTreeState(*this, nullptr, "Params", createParameterLayout());
    
    noiseGate = new NoiseGate();
    preBoost  = new Boost();
    preamp    = new PreampDistorsion();
    toneStack = new Tonestack();
    irLoader  = new IRLoader();

    for (uint8_t i = 0; i < N_PARAMS; i++) {
        apvts->addParameterListener(ParamIDs[i], this);
    }
}

Processor::~Processor() {

    for (uint8_t i = 0; i < N_PARAMS; i++) {
        apvts->removeParameterListener(ParamIDs[i], this);
    }

    delete noiseGate;
    delete preBoost;
    delete preamp;
    delete toneStack;
    delete irLoader;

    if (intputSignalCopy) {
        free(intputSignalCopy);
    }

    delete apvts;
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

    inputNoiseFilter.setCoefficients(3000.0, 0.7, 0.0, sampleRate);

    noiseGate->prepareToPlay(sampleRate);
    preBoost->prepareToPlay();

    preamp->prepareToPlay(sampleRate, samplesPerBlock);

    masterVolume.init(1.0);
    masterVolume.newTarget(DB_TO_GAIN(-6.0f), SMOOTH_PARAM_TIME, samplerate);

    toneStack->prepareToPlay(sampleRate);
    irLoader->init(sampleRate, samplesPerBlock);

    resonanceFilter.setCoefficients(RESONANCE_FREQUENCY, RESONANCE_Q, 0.0, sampleRate);

    presenceFilter.setCoefficients(PRESENCE_FREQUENCY, PRESENCE_Q, 0.0, sampleRate);

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
    juce::MemoryOutputStream stream(destData, false);
    apvts->state.writeToStream(stream);
}   

void Processor::setStateInformation (const void* data, int sizeInBytes) {
    // Recall params
    juce::ValueTree importedTree = juce::ValueTree::readFromData(data, size_t(sizeInBytes));

    if (importedTree.isValid()) {
        apvts->state = importedTree;
        initParameters();
    }

}

void Processor::initParameters() {

    noiseGate->threshold = DB_TO_GAIN(*apvts->getRawParameterValue(ParamIDs[GATE_THRESH]));
                               
    preBoost->tightFilter.setCoefficients(*apvts->getRawParameterValue(ParamIDs[TIGHT]), samplerate);
    preBoost->biteFilter.setCoefficients(BOOST_BITE_FREQ, BOOST_BITE_Q, 
                                          *apvts->getRawParameterValue(ParamIDs[BITE]), 
                                          samplerate);


    preamp->inputFilter.setCoefficients(*apvts->getRawParameterValue(ParamIDs[INPUT_FILTER]), 
                                        samplerate*PREAMP_UP_SAMPLE_FACTOR);
                                        
    auto paramRange = apvts->getParameter(ParamIDs[PREAMP_GAIN])->getNormalisableRange();
    float paramValue = *apvts->getRawParameterValue(ParamIDs[PREAMP_GAIN]);
    preamp->preGain.newTarget(scale(paramValue, paramRange.start, paramRange.end, 0.0f, 1.0f, 2.0f),
                                SMOOTH_PARAM_TIME, 
                                samplerate * PREAMP_UP_SAMPLE_FACTOR);

    ToneStackModel model = static_cast<ToneStackModel>((int)*apvts->getRawParameterValue(ParamIDs[TONESTACK_MODEL]) - 1);
    toneStack->comp->setModel(model);

    float bassEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_BASS]);
    float trebbleEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_TREBBLE]);
    float midEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_MIDDLE]);
    toneStack->updateCoefficients(trebbleEQgain, midEQgain, bassEQgain, samplerate);


    preamp->postGain.newTarget(*apvts->getRawParameterValue(ParamIDs[PREAMP_VOLUME]), 
                               SMOOTH_PARAM_TIME, 
                               samplerate);


    resonanceFilter.setCoefficients(RESONANCE_FREQUENCY, RESONANCE_Q, 
                                     *apvts->getRawParameterValue(ParamIDs[RESONANCE]), 
                                     samplerate);
                                     
    presenceFilter.setCoefficients(PRESENCE_FREQUENCY, PRESENCE_Q, 
                                    *apvts->getRawParameterValue(ParamIDs[PRESENCE]), 
                                    samplerate);
    
    masterVolume.newTarget(*apvts->getRawParameterValue(ParamIDs[MASTER_VOLUME]), 
                            SMOOTH_PARAM_TIME, samplerate);
}

void Processor::parameterChanged(const juce::String &parameterID, float newValue) {

    // int paramIndex = 0;

    // for(int i = 0; i < ParamIDs.size(); i++) {
    //     if (parameterID == ParamIDs[i]) {
    //         paramIndex = i;
    //         break;
    //     }
    // }
    //
    // switch (paramIndex) {}


    if (parameterID == ParamIDs[GATE_THRESH]) {
        noiseGate->threshold = DB_TO_GAIN(newValue);
        return;
    }

    if (parameterID == ParamIDs[BITE]) {
        preBoost->biteFilter.setCoefficients(BOOST_BITE_FREQ, BOOST_BITE_Q, newValue, samplerate);

        return;
    }

    if (parameterID == ParamIDs[TIGHT]) {
        preBoost->tightFilter.setCoefficients(newValue, samplerate);
        return;
    }

    if (parameterID == ParamIDs[INPUT_FILTER]) {
        preamp->inputFilter.setCoefficients(newValue, samplerate*PREAMP_UP_SAMPLE_FACTOR);
        return;
    }

    if (parameterID == ParamIDs[PREAMP_GAIN]) {
        auto paramRange = apvts->getParameter(parameterID)->getNormalisableRange();

        preamp->preGain.newTarget(scale(newValue, paramRange.start, paramRange.end, 0.0f, 1.0f, 2.0f),
                                  SMOOTH_PARAM_TIME, 
                                  samplerate * PREAMP_UP_SAMPLE_FACTOR);
        return;
    }

    if (parameterID == ParamIDs[CHANNEL]) {
        
        preamp->channel = (uint8_t)newValue;
        return;
    }

    if (parameterID == ParamIDs[PREAMP_VOLUME]) {

        preamp->postGain.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate * PREAMP_UP_SAMPLE_FACTOR);

        return;
    }        

    if (parameterID == ParamIDs[TONESTACK_BASS]
        || parameterID == ParamIDs[TONESTACK_MIDDLE]
        || parameterID == ParamIDs[TONESTACK_TREBBLE]
        || parameterID == ParamIDs[TONESTACK_MODEL])
    {
        ToneStackModel model = static_cast<ToneStackModel>((int)*apvts->getRawParameterValue(ParamIDs[TONESTACK_MODEL]) - 1);
        toneStack->comp->setModel(model);
        
        float bassEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_BASS]);
        float trebbleEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_TREBBLE]);
        float midEQgain = *apvts->getRawParameterValue(ParamIDs[TONESTACK_MIDDLE]);
        toneStack->updateCoefficients(trebbleEQgain, midEQgain, bassEQgain, samplerate);
        return;
    }

    if (parameterID == ParamIDs[RESONANCE]) {
        resonanceFilter.setCoefficients(RESONANCE_FREQUENCY, RESONANCE_Q, newValue, samplerate);
        return;
    }

    if (parameterID == ParamIDs[PRESENCE]) {
        presenceFilter.setCoefficients(PRESENCE_FREQUENCY, PRESENCE_Q, newValue, samplerate);
        return;
    }

    if (parameterID == ParamIDs[MASTER_VOLUME]) {
        masterVolume.newTarget(newValue, SMOOTH_PARAM_TIME, samplerate);
        return;
    }

}

juce::AudioProcessorValueTreeState::ParameterLayout Processor::createParameterLayout()
{   

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[GATE_THRESH], "Gate Thresh", -90.0f, -40.0f, -75.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[BITE], "Bite", 0.0f, 30.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[TIGHT], "Tight", 20.0f, 1000.0f, 0.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_GAIN], "Pre Gain", 0.0f, 10.0f, 5.0f
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[INPUT_FILTER], "Input Filter", 0.0f, 700.0f, 50.0f
    ));
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        ParamIDs[CHANNEL], "Amp Channel", 1, 4, 2
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
    
    params.push_back(std::make_unique<juce::AudioParameterInt>(
        ParamIDs[TONESTACK_MODEL], "Tonestack model", 1, 4, 1
    ));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PREAMP_VOLUME], "Post Gain", -36.0f, 12.0f, 0.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[RESONANCE], "Reson", 0.0f, 6.0f, 3.0f
    ));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        ParamIDs[PRESENCE], "Presence", 0.0f, 12.0f, 6.0f
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
    return new Processor();
}
