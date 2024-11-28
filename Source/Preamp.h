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

constexpr u8 PREAMP_UP_SAMPLE_FACTOR = 8;

struct Preamp {
    Preamp();
    ~Preamp();
    
    void prepareToPlay(double samplerate, u32 blockSize);
    void setBias(float value, int tube_index);  
    void process(Sample *bufferL, Sample *bufferR, u32 nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;

    OnepoleFilter inputFilter;
    
    FirstOrderShelfFilter brightCapFilter {lowshelf};
    
    OnepoleFilter couplingFilter1;
    OnepoleFilter couplingFilter2;
    OnepoleFilter couplingFilter3;
    OnepoleFilter couplingFilter4;

    OnepoleFilter stageOutputFilter0;
    OnepoleFilter stageOutputFilter1;
    OnepoleFilter stageOutputFilter2;
    OnepoleFilter stageOutputFilter3;
    OnepoleFilter stageOutputFilter4;

    FirstOrderShelfFilter cathodeBypassFilter0 {lowshelf};
    FirstOrderShelfFilter cathodeBypassFilter1 {lowshelf};
    FirstOrderShelfFilter cathodeBypassFilter2 {lowshelf};
    FirstOrderShelfFilter cathodeBypassFilter3 {lowshelf};
    FirstOrderShelfFilter cathodeBypassFilter4 {lowshelf};

    struct {
        Biquad upSampleFilter1 {BIQUAD_LOWPASS};
        Biquad upSampleFilter2 {BIQUAD_LOWPASS};
        
        Biquad downSampleFilter1 {BIQUAD_LOWPASS};
        Biquad downSampleFilter2 {BIQUAD_LOWPASS};
    } overSampler;
    
    Sample *upBufferL = nullptr;
    Sample *upBufferR = nullptr;

    float stage0_bias[2] = {0};
    float stage1_bias[2] = {0};
    float stage2_bias[2] = {0};
    float stage3_bias[2] = {0};
    float stage4_bias[2] = {0};

    Sample outputAttenuationdB = -40.0f;    
    u8 channel = 0;
};

#endif // PREAMP_H
