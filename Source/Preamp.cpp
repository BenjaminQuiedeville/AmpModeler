/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "Preamp.h"

Preamp::~Preamp() {
    free(upSampledBufferL);
}

void Preamp::prepareToPlay(float samplerate, u32 blockSize) {

    float upSamplerate = samplerate*PREAMP_UP_SAMPLE_FACTOR;
    u32 upBlockSize = blockSize * PREAMP_UP_SAMPLE_FACTOR;

    preGain.init(0.0f, upBlockSize);
    postGain.init(dbtoa(-60.0f), upBlockSize);

    stage1Gain.init(0.0f, upBlockSize);
    stage2Gain.init(0.0f, upBlockSize);
    stage3Gain.init(0.0f, upBlockSize);


    inputMudFilter.reset();
    inputMudFilter.makeHighpass(100.0, upSamplerate);

    midBoost.reset();
    midBoost.setCoefficients(BIQUAD_PEAK, 1000.0, 0.2, 3.0, upSamplerate);

    inputFilter.reset();
    inputFilter.makeHighpass(100.0f, upSamplerate);
    
    couplingFilter1.reset();
    couplingFilter2.reset();
    couplingFilter3.reset();
    couplingFilter4.reset();
    couplingFilter1.makeHighpass(15.0, upSamplerate);
    couplingFilter2.makeHighpass(15.0, upSamplerate);
    couplingFilter3.makeHighpass(15.0, upSamplerate);
    couplingFilter4.makeHighpass(15.0, upSamplerate);

    stage0LP.reset();
    stage1LP.reset();
    stage2LP.reset();
    stage3LP.reset();
    stage4LP.reset();

    cathodeBypassFilter0.reset();
    cathodeBypassFilter1.reset();
    cathodeBypassFilter2.reset();
    cathodeBypassFilter3.reset();
    cathodeBypassFilter4.reset();

    overSampler.upSampleFilter1.reset();
    overSampler.upSampleFilter2.reset();
    overSampler.downSampleFilter1.reset();
    overSampler.downSampleFilter2.reset();

    // earlevel.com/main/2016/09/29/cascading-filters
    overSampler.upSampleFilter1.setCoefficients(BIQUAD_LOWPASS, samplerate/2 * 0.9, 0.54119610, 0.0, upSamplerate);
    overSampler.upSampleFilter2.setCoefficients(BIQUAD_LOWPASS, samplerate/2 * 0.9, 1.3065630, 0.0, upSamplerate);
    overSampler.downSampleFilter1.setCoefficients(BIQUAD_LOWPASS, samplerate/2 * 0.9, 0.54119610, 0.0, upSamplerate);
    overSampler.downSampleFilter2.setCoefficients(BIQUAD_LOWPASS, samplerate/2 * 0.9, 1.3065630, 0.0, upSamplerate);

    if (upSampledBufferL) {
        upSampledBufferL = (float *)realloc(upSampledBufferL, upBlockSize * sizeof(float) * 2);
        upSampledBufferR = upSampledBufferL + upBlockSize;
    } else {
        upSampledBufferL = (float *)calloc(upBlockSize * 2,  sizeof(float));
        upSampledBufferR = upSampledBufferL + upBlockSize;
    }
}

void Preamp::setBias(float bias, int tube_index) {
    ZoneScoped;
    // @TODO: add smoothing to avoid clicks
    float *selected_stage_bias = nullptr;
    switch (tube_index) {
    
        case 0: { selected_stage_bias = stage0_bias; break; }
        case 1: { selected_stage_bias = stage1_bias; break; }
        case 2: { selected_stage_bias = stage2_bias; break; }
        case 3: { selected_stage_bias = stage3_bias; break; }
        case 4: { selected_stage_bias = stage4_bias; break; }
        default: { assert(false && "setBias: wrong tube_index"); }
    }

    static const float positiveLinRange = 0.2f;

    selected_stage_bias[0] = bias;

    if (bias > positiveLinRange) {
        // selected_stage_bias[1] = tanh(bias - positiveLinRange) + positiveLinRange;
        // selected_stage_bias[1] = 1.0f - expf(positiveLinRange - bias) + positiveLinRange;
        float temp = (bias-positiveLinRange) * 0.5f + 1.0f;
        selected_stage_bias[1] = 1.0f - 1.0f/(temp*temp) + positiveLinRange;
    } else {
        selected_stage_bias[1] = bias;
    }
}

// auto cubicClip = [](float x) { return x < -1.0f ? -2.0f/3.0f : x - 1.0f/3.0f * x*x*x; };

static void gridConduction(float *bufferL, float *bufferR, u32 nSamples, float ratio) {
    ZoneScoped;

    // ratio 8 ou 4

    constexpr float threshold = 1.5f;
    float inv_ratio = 1.0f/ratio;
    float knee = ratio / 4.0f;
    float inv_2_knee = 1.0f/(2.0f*knee);
    // @TODO: pour le bias, puis utiliser un Onepole pour smooth l'offset calculé sur ~10ms
    // refaire une fonction de calcul d'un seul echantillon pour les onepole

    float *buffers[2] = {bufferL, bufferR};    
    u32 nChannels = bufferR ? 2 : 1;

    for (u32 channelIndex = 0; channelIndex < nChannels; channelIndex++) {

        for (u32 index = 0; index < nSamples; index++) {
            float sample = buffers[channelIndex][index];
    
            if (2.0f * (sample - threshold) > knee) {
                sample = threshold + (sample - threshold)*inv_ratio;
                        // sample = thresh + sample/ratio - thresh/ratio
                        // sample = thresh*ratio + sample - thresh
                        // sample = sample + thresh*ratio - thresh
            } else if (2.0f * abs(sample - threshold) <= knee) {
                float temp = sample - threshold + knee * 0.5f;
                sample += (inv_ratio - 1.0f) * temp*temp * inv_2_knee;
            }
        
            if (sample > 0.0f) {
                constexpr float negClipPoint = 3.0f;
    
                sample *= 2.0f/(3.0f*negClipPoint);
                sample = sample > 1.0f ? 2.0f/3.0f : sample - 1.0f/3.0f * sample*sample*sample;
                sample *= negClipPoint * 1.5f;
            }
            
            buffers[channelIndex][index] = sample;
        }
    }
}

static void tubeSim(float *bufferL, float *bufferR, u32 nSamples, float pre_bias, float post_bias) {
    ZoneScoped;
    
    float *buffers[2] = {bufferL, bufferR};    
    u32 nChannels = bufferR ? 2 : 1;
    
    for (u32 channelIndex = 0; channelIndex < nChannels; channelIndex++) {
        for (u32 index = 0; index < nSamples; index++) {
            float sample = buffers[channelIndex][index];
            
            sample *= -1.0f;
            sample += pre_bias;
    
            constexpr float positiveLinRange = 0.5f;
            constexpr float negClipPoint = 3.0f;
                
            if (sample > positiveLinRange) {
                // sample = tanh(sample - positiveLinRange) + positiveLinRange;
                // sample = 1.0f - expf(positiveLinRange - sample) + positiveLinRange;
                float temp = (sample-positiveLinRange) * 0.5f + 1.0f;
                sample = 1.0f - 1.0f/(temp*temp) + positiveLinRange;
            }

            buffers[channelIndex][index] = (sample - post_bias);
        }
    }
}


void Preamp::process(float *bufferL, float *bufferR, u32 nSamples) {
    ZoneScopedN("Preamp");
    
    u32 upNumSamples = nSamples*PREAMP_UP_SAMPLE_FACTOR;

    float* upBufferL = upSampledBufferL;
    float* upBufferR = nullptr;
    if (bufferR) {
        upBufferR = upSampledBufferR;
    }

    {
        ZoneScopedN("Upsampling");

        FLOAT_CLEAR(upBufferL, upNumSamples);
        for (u32 i = 0; i < nSamples; i++) {
            upBufferL[PREAMP_UP_SAMPLE_FACTOR*i] = bufferL[i];
        }
    
        if (bufferR) {
            FLOAT_CLEAR(upBufferR, upNumSamples);
            for (u32 i = 0; i < nSamples; i++) {
                upBufferR[PREAMP_UP_SAMPLE_FACTOR*i] = bufferR[i];
            }
        }
    
        overSampler.upSampleFilter1.process(upBufferL, upBufferR, upNumSamples);
        overSampler.upSampleFilter2.process(upBufferL, upBufferR, upNumSamples);
        applyGainLinear(PREAMP_UP_SAMPLE_FACTOR, upBufferL, upBufferR, upNumSamples);
    }


    {
        ZoneScopedN("GainsStages");
        
        // Input Gain
        // static const float INPUT_GAIN = (float)dbtoa(0.0);
        // applyGainLinear(INPUT_GAIN, upBufferL, upBufferR, upNumSamples);

        // ------------ Stage 0 ------------
        gridConduction(upBufferL, upBufferR, upNumSamples, 2.0f);        
        cathodeBypassFilter0.process(upBufferL, upBufferR, upNumSamples);        
        tubeSim(upBufferL, upBufferR, upNumSamples, stage0_bias[0], stage0_bias[1]);

        inputFilter.process(upBufferL, upBufferR, upNumSamples);
        stage0LP.process(upBufferL, upBufferR, upNumSamples);

        preGain.applySmoothGainLinear(upBufferL, upBufferR, upNumSamples);
        inputMudFilter.process(upBufferL, upBufferR, upNumSamples);
        midBoost.process(upBufferL, upBufferR, upNumSamples);

        if (bright) {
            brightCapFilter.process(upBufferL, upBufferR, upNumSamples);
        }


        // ------------ Stage 1 ------------
        gridConduction(upBufferL, upBufferR, upNumSamples, 4.0f);
        if (channel != 1) { 
            cathodeBypassFilter1.process(upBufferL, upBufferR, upNumSamples); 
        }
        tubeSim(upBufferL, upBufferR, upNumSamples, stage1_bias[0], stage1_bias[1]);
        
        couplingFilter1.process(upBufferL, upBufferR, upNumSamples);
        stage1LP.process(upBufferL, upBufferR, upNumSamples);
    
        if (channel == 1) {
            goto gain_stages_end_of_scope;
        }

        stage1Gain.applySmoothGainLinear(upBufferL, upBufferR, upNumSamples);


        // ------------ Stage 2 ------------
        gridConduction(upBufferL, upBufferR, upNumSamples, 4.0f);
        if (channel != 2) { cathodeBypassFilter2.process(upBufferL, upBufferR, upNumSamples); }
        tubeSim(upBufferL, upBufferR, upNumSamples, stage2_bias[0], stage2_bias[1]);
        
        couplingFilter2.process(upBufferL, upBufferR, upNumSamples);
        stage2LP.process(upBufferL, upBufferR, upNumSamples);

        if (channel == 2) {
            goto gain_stages_end_of_scope;
        }

        stage2Gain.applySmoothGainLinear(upBufferL, upBufferR, upNumSamples);


        // ------------ Stage 3 ------------
        gridConduction(upBufferL, upBufferR, upNumSamples, 4.0f);
        if (channel != 3) { cathodeBypassFilter3.process(upBufferL, upBufferR, upNumSamples); }
        tubeSim(upBufferL, upBufferR, upNumSamples, stage3_bias[0], stage3_bias[1]);
        
        couplingFilter3.process(upBufferL, upBufferR, upNumSamples);
        stage3LP.process(upBufferL, upBufferR, upNumSamples);

        if (channel == 3) {
            goto gain_stages_end_of_scope;
        }

        stage3Gain.applySmoothGainLinear(upBufferL, upBufferR, upNumSamples);


        // ------------ Stage 4 ------------
        gridConduction(upBufferL, upBufferR, upNumSamples, 8.0f);
        if (channel != 4) { cathodeBypassFilter4.process(upBufferL, upBufferR, upNumSamples); }
        tubeSim(upBufferL, upBufferR, upNumSamples, stage4_bias[0], stage4_bias[1]);
        
        couplingFilter4.process(upBufferL, upBufferR, upNumSamples);
        stage4LP.process(upBufferL, upBufferR, upNumSamples);

        gain_stages_end_of_scope: {}
    }

    postGain.applySmoothGainLinear(upBufferL, upBufferR, upNumSamples);


    {
        ZoneScopedN("DownSampling");
        assert(upNumSamples == nSamples*PREAMP_UP_SAMPLE_FACTOR);
        
        overSampler.downSampleFilter1.process(upBufferL, upBufferR, upNumSamples);
        overSampler.downSampleFilter2.process(upBufferL, upBufferR, upNumSamples);
    
        for (u32 i = 0; i < nSamples; i++) {
            bufferL[i] = upBufferL[i*PREAMP_UP_SAMPLE_FACTOR];
        }
        if (bufferR) {
            for (u32 i = 0; i < nSamples; i++) {
                bufferR[i] = upBufferR[i*PREAMP_UP_SAMPLE_FACTOR];
            }
        }
    }
}
