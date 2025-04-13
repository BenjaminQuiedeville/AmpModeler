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

static const u8 PREAMP_UP_SAMPLE_FACTOR = 8;
static const float tube_gain = 100.0f;

struct Preamp {
    ~Preamp();
    
    void prepareToPlay(double samplerate, u32 blockSize);
    void setBias(float value, int tube_index);  
    void process(float *bufferL, float *bufferR, u32 nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;

    SmoothParamLinear stage1Gain;
    SmoothParamLinear stage2Gain;
    SmoothParamLinear stage3Gain;

    OnepoleFilter inputFilter;
    OnepoleFilter inputMudFilter;
    Biquad midBoost {BIQUAD_PEAK};
    
    FirstOrderShelfFilter brightCapFilter {lowshelf};
    
    OnepoleFilter couplingFilter1;
    OnepoleFilter couplingFilter2;
    OnepoleFilter couplingFilter3;
    OnepoleFilter couplingFilter4;

    OnepoleFilter stage0LP;
    OnepoleFilter stage1LP;
    OnepoleFilter stage2LP;
    OnepoleFilter stage3LP;
    OnepoleFilter stage4LP;

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
    
    float *upSampledBufferL = nullptr;
    float *upSampledBufferR = nullptr;

    float stage0_bias[2] = {0};
    float stage1_bias[2] = {0};
    float stage2_bias[2] = {0};
    float stage3_bias[2] = {0};
    float stage4_bias[2] = {0};

    float outputAttenuationdB = -34.0f;
    u8 channel = 0;
    bool bright = true;
};

#endif // PREAMP_H
