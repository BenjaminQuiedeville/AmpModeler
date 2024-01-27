/*
  ==============================================================================

    PreampDistorsion.h
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef PREAMP_H
#define PREAMP_H

#include "juce_dsp/juce_dsp.h"

#include "common.h"
#include "OnepoleFilter.h"
#include "Biquad.h"
#include "SmoothParam.h"

/*

    faire mon propre algo d'oversampling avec la sincTable
    changer l'algo de distorsion pour une courbe asymétrique haute headroom
*/

struct OverSampler {

    OverSampler();

    ~OverSampler();

    void prepareToPlay(double _samplerate);

    void upSample(sample_t *source, sample_t *upSampled, size_t sourceSize, size_t upSampledSize);
    void downSample(sample_t *upSampled, sample_t *dest, size_t upSampledSize, size_t destSize);

    Biquad *upSampleFilter1;
    Biquad *upSampleFilter2;
    
    Biquad *downSampleFilter1;
    Biquad *downSampleFilter2;

    uint8_t upSampleFactor = 4;

};


struct PreampDistorsion {
    PreampDistorsion();
    ~PreampDistorsion();
    
    void prepareToPlay(double samplerate, int blockSize);
    void process(float *buffer, size_t nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;
    double samplerate = 0.0;
    sample_t stageGain = 0.0;
    sample_t outputAttenuation = (sample_t)DB_TO_GAIN(-20.0);

    OnepoleFilter inputFilter;

    OnepoleFilter couplingFilter1;
    OnepoleFilter couplingFilter2;
    OnepoleFilter couplingFilter3;

    OnepoleFilter stageOutputFilter1;
    OnepoleFilter stageOutputFilter2;

    Biquad *tubeBypassFilter1 = nullptr;
    Biquad *tubeBypassFilter2 = nullptr;

    juce::dsp::Oversampling<sample_t> *overSampler;
    AudioBlock overSampledBlock;
    uint8_t upSampleFactor = 4;
    sample_t headroom = 10.0f;

};

#endif // PREAMP_H