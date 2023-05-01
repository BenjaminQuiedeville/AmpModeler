/*
  ==============================================================================

    PreampDistorsion.cpp
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreampDistorsion.h"

PreampDistorsion::PreampDistorsion() : 
    overSampler(2, 2, juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR)
{

    preGain.setGainDecibels(0.0f);
    postGain.setGainDecibels(-36.0f);

}

void PreampDistorsion::prepareToPlay(juce::dsp::ProcessSpec &spec)
{
    m_samplerate = spec.sampleRate;
    overSampler.initProcessing(spec.maximumBlockSize);

}

void PreampDistorsion::process(AudioBlock &audioblock)
{
    // d'abord dans upsampling pour voir si la courbe fonctionne bien.
}
