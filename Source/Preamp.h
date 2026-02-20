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

    FirstOrderFilter cathodeBypassFilter1;
    float stage1_bias[2] = {0};
    FirstOrderFilter inputFilter;
    FirstOrderFilter stage1LP;
    SmoothParamLinear stage1Gain;
    FirstOrderFilter inputMudFilter;
    Biquad midBoost;
    FirstOrderFilter brightCapFilter;


    FirstOrderFilter cathodeBypassFilter2;
    float stage2_bias[2] = {0};
    FirstOrderFilter couplingFilter2;
    FirstOrderFilter stage2LP;
    SmoothParamLinear stage2Gain;


    FirstOrderFilter cathodeBypassFilter3;
    float stage3_bias[2] = {0};
    FirstOrderFilter couplingFilter3;
    FirstOrderFilter stage3LP;
    SmoothParamLinear stage3Gain;


    FirstOrderFilter cathodeBypassFilter4;
    float stage4_bias[2] = {0};
    FirstOrderFilter couplingFilter4;
    FirstOrderFilter stage4LP;
    SmoothParamLinear stage4Gain;
    
    FirstOrderFilter couplingFilter5;
    float stage5_bias[2] = {0};
    FirstOrderFilter stage5LP;
    
    SmoothParamLinear volume;

    struct {
        Biquad upSampleFilter1;
        Biquad upSampleFilter2;
        
        Biquad downSampleFilter1;
        Biquad downSampleFilter2;
    } overSampler;
    
    float *upSampledBufferL = nullptr;
    float *upSampledBufferR = nullptr;


    float outputAttenuationdB = -34.0f;
    u8 channel = 0;
    bool bright = true;
};

#endif // PREAMP_H
