/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef PREAMP_H
#define PREAMP_H

#include "common.h"
#include "OnepoleFilter.h"
#include "Biquad.h"
#include "SmoothParam.h"
#include <assert.h>
#include <memory>

constexpr u8 PREAMP_UP_SAMPLE_FACTOR = 4;

struct OverSampler {

    Biquad upSampleFilter1 {BIQUAD_LOWPASS};
    Biquad upSampleFilter2 {BIQUAD_LOWPASS};
    
    Biquad downSampleFilter1 {BIQUAD_LOWPASS};
    Biquad downSampleFilter2 {BIQUAD_LOWPASS};
};


struct Preamp {
    Preamp();
    ~Preamp();
    
    void prepareToPlay(double samplerate, int blockSize);
    void processGainStages(Sample *buffer, size_t nSamples);
    void process(Sample *buffer, size_t nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;

    OnepoleFilter inputFilter;
    
    Biquad brightCapFilter {BIQUAD_LOWSHELF};
    
    OnepoleFilter couplingFilter1;
    OnepoleFilter couplingFilter2;
    OnepoleFilter couplingFilter3;
    OnepoleFilter couplingFilter4;

    OnepoleFilter stageOutputFilter1;
    OnepoleFilter stageOutputFilter2;
    OnepoleFilter stageOutputFilter3;
    OnepoleFilter stageOutputFilter4;

    Biquad cathodeBypassFilter1 {BIQUAD_LOWSHELF};
    Biquad cathodeBypassFilter2 {BIQUAD_LOWSHELF};
    Biquad cathodeBypassFilter3 {BIQUAD_LOWSHELF};
    Biquad cathodeBypassFilter4 {BIQUAD_LOWSHELF};

    OverSampler *overSampler;
    Sample *upSampledBlock = nullptr;

    u8 channel = 0;

};

#endif // PREAMP_H