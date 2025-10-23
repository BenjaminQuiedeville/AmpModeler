/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef PREAMP_H
#define PREAMP_H

#include "common.h"
#include "FirstOrderFilter.h"
#include "Biquad.h"
#include "SmoothParam.h"
#include <assert.h>
#include <memory>

static const u8 PREAMP_UP_SAMPLE_FACTOR = 8;
static const float tube_gain = 70.0f;

struct Preamp {
    ~Preamp();
    
    void prepareToPlay(float samplerate, u32 blockSize); 
    void setBias(float value, int tube_index);  
    void process(float *bufferL, float *bufferR, u32 nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;

    SmoothParamLinear stage1Gain;
    SmoothParamLinear stage2Gain;
    SmoothParamLinear stage3Gain;

    FirstOrderFilter inputFilter;
    FirstOrderFilter inputMudFilter;
    Biquad midBoost;
    
    FirstOrderFilter brightCapFilter;
    
    FirstOrderFilter couplingFilter1;
    FirstOrderFilter couplingFilter2;
    FirstOrderFilter couplingFilter3;
    FirstOrderFilter couplingFilter4;

    FirstOrderFilter stage0LP;
    FirstOrderFilter stage1LP;
    FirstOrderFilter stage2LP;
    FirstOrderFilter stage3LP;
    FirstOrderFilter stage4LP;

    FirstOrderFilter cathodeBypassFilter0;
    FirstOrderFilter cathodeBypassFilter1;
    FirstOrderFilter cathodeBypassFilter2;
    FirstOrderFilter cathodeBypassFilter3;
    FirstOrderFilter cathodeBypassFilter4;

    struct {
        Biquad upSampleFilter1;
        Biquad upSampleFilter2;
        
        Biquad downSampleFilter1;
        Biquad downSampleFilter2;
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
