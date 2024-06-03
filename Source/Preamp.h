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
#include "FirstOrderShelf.h"

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
    
    void prepareToPlay(double samplerate, u32 blockSize);
    void processGainStagesMono(Sample *buffer, u32 nSamples);
    void processGainStagesStereo(Sample *bufferL, Sample *bufferR, u32 nSamples);
    void processMono(Sample *buffer, u32 nSamples);
    void processStereo(Sample *bufferL, Sample *bufferR, u32 nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;

    OnepoleFilter inputFilter;
    
    FirstOrderShelfFilter brightCapFilter {lowshelf};
    
    OnepoleFilter couplingFilter1;
    OnepoleFilter couplingFilter2;
    OnepoleFilter couplingFilter3;
    OnepoleFilter couplingFilter4;

    OnepoleFilter stageOutputFilter1;
    OnepoleFilter stageOutputFilter2;
    OnepoleFilter stageOutputFilter3;
    OnepoleFilter stageOutputFilter4;

    FirstOrderShelfFilter cathodeBypassFilter1 {lowshelf};
    FirstOrderShelfFilter cathodeBypassFilter2 {lowshelf};
    FirstOrderShelfFilter cathodeBypassFilter3 {lowshelf};
    FirstOrderShelfFilter cathodeBypassFilter4 {lowshelf};

    OverSampler *overSampler;
    Sample *upSampledBlockL = nullptr;
    Sample *upSampledBlockR = nullptr;

    u8 channel = 0;

};

#endif // PREAMP_H