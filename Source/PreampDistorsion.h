/*
  ==============================================================================

    PreampDistorsion.h
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef PREAMP_H
#define PREAMP_H

#include <JuceHeader.h>

#include "common.h"
#include "OnepoleFilter.h"
#include "Biquad.h"
#include "SmoothParam.h"

/*

    faire mon propre algo d'oversampling avec la sincTable
    changer l'algo de distorsion pour une courbe asymétrique haute headroom
*/


struct PreampDistorsion {
    PreampDistorsion();
    ~PreampDistorsion();
    
    void prepareToPlay(double samplerate, int blockSize);
    void process(float *buffer, size_t nSamples);

    SmoothParam *preGain;
    SmoothParam *postGain;
    double stageGain;
    double samplerate;
    double outputAttenuation = DB_TO_GAIN(-25.0);

    OnepoleFilter *inputFilter;

    OnepoleFilter *couplingFilter1;
    OnepoleFilter *couplingFilter2;
    OnepoleFilter *couplingFilter3;

    Biquad *tubeBypassFilter1;
    Biquad *tubeBypassFilter2;

    juce::dsp::Oversampling<sample_t> *overSampler;
    AudioBlock overSampledBlock;
    int upSampleFactor;


};

    // sample_t expappr(sample_t x) {

    //     sample_t x2 = x*x;
    //     sample_t x3 = x2*x;
    //     sample_t x4 = x3*x;
    //     sample_t x5 = x4*x;
    //     return 1.0f + x + x2/2.0f + x3/6.0f + x4/24.0f + x5/120.0f; 
    // }

    // sample_t tanhApprox(sample_t x) {
    //     return (expappr(x) - expappr(-x))/(expappr(x) + expappr(-x)); 
    // }
#endif // PREAMP_H