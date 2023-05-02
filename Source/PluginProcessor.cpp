/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

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
}

AmpModelerAudioProcessor::~AmpModelerAudioProcessor()
{
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
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    preamp.prepareToPlay(spec);

    masterVolume.prepare(spec);
    masterVolume.setRampDurationSeconds(0.02f);

    postEQ.prepareToPlay(spec);
    irLoader.prepareToPlay(spec);
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

    size_t numSamples = buffer.getNumSamples();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i) {
        buffer.clear (i, 0, numSamples);
    }

    AudioBlock audioBlock { buffer };

    preamp.preGain.setGainDecibels(*apvts.getRawParameterValue("PREAMP_GAIN"));

    float bassEQgain = *apvts.getRawParameterValue("3_BAND_EQ_BASS");
    float midEQgian = *apvts.getRawParameterValue("3_BAND_EQ_MIDDLE");
    float trebbleEQgian = *apvts.getRawParameterValue("3_BAND_EQ_TREBBLE");
    postEQ.updateGains(bassEQgain, midEQgian, trebbleEQgian);
    
    masterVolume.setGainDecibels(*apvts.getRawParameterValue("MASTER_VOLUME"));

    /******PROCESS********/

    preamp.process(audioBlock);
    postEQ.process(audioBlock);
    irLoader.performConvolution(audioBlock);
    // safetyClip(audioBlock);
    

    masterVolume.process(juce::dsp::ProcessContextReplacing<sample_t>(audioBlock));

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
void AmpModelerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AmpModelerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void AmpModelerAudioProcessor::safetyClip(AudioBlock &audioBlock) {

    auto clip = [](sample_t x) {
        return x > 1.0f ? 1.0f
             : x < -1.0f ? -1.0f 
             : x;
    };

    for (size_t channelIndex = 0; channelIndex < audioBlock.getNumChannels(); channelIndex++) {
        sample_t *bufferPtr = audioBlock.getChannelPointer(channelIndex);

        for (size_t index = 0; index < audioBlock.getNumSamples(); index++) {
            
            bufferPtr[index] = clip(bufferPtr[index]);
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout AmpModelerAudioProcessor::createParameterLayout()
{   

    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("MASTER_VOLUME", "Master Vol", -20.0f, 0.0f, -6.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("3_BAND_EQ_BASS", "Bass", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("3_BAND_EQ_MIDDLE", "Mid", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("3_BAND_EQ_TREBBLE", "Trebble", -12.0f, 12.0f, 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>("PREAMP_GAIN", "Preamp Gain", -30.3f, 30.0f, 0.0f));
    

    return { params.begin(), params.end() };
}




//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AmpModelerAudioProcessor();
}
