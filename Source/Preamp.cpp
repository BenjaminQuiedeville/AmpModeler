/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "Preamp.h"

Preamp::~Preamp() {
    free(upSampledBuffer.dataL);
}

void Preamp::prepareToPlay(float samplerate, u32 blockSize) {

    float upSamplerate = samplerate*PREAMP_UP_SAMPLE_FACTOR;
    u32 upBlockSize = blockSize * PREAMP_UP_SAMPLE_FACTOR;


    stage1Gain.init(0.0f, upBlockSize);
    stage2Gain.init(0.0f, upBlockSize);
    stage3Gain.init(0.0f, upBlockSize);
    stage4Gain.init(0.0f, upBlockSize);
    
    volume.init(dbtoa(-60.0f), upBlockSize);

    inputMudFilter.reset();
    inputMudFilter.makeHighpass(100.0, upSamplerate);

    midBoost.reset();
    midBoost.setCoefficients(BIQUAD_PEAK, 1000.0, 0.2, 3.0, upSamplerate);

    inputFilter.reset();
    inputFilter.makeHighpass(100.0f, upSamplerate);
    
    couplingFilter2.reset();
    couplingFilter3.reset();
    couplingFilter4.reset();
    couplingFilter5.reset();
    couplingFilter2.makeHighpass(15.0, upSamplerate);
    couplingFilter3.makeHighpass(15.0, upSamplerate);
    couplingFilter4.makeHighpass(15.0, upSamplerate);
    couplingFilter5.makeHighpass(15.0, upSamplerate);

    stage1LP.reset();
    stage2LP.reset();
    stage3LP.reset();
    stage4LP.reset();
    stage5LP.reset();

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

    if (upSampledBuffer.dataL) { free(upSampledBuffer.dataL); }
    upSampledBuffer.size = upBlockSize;
    upSampledBuffer.dataL = allocFloat(upSampledBuffer.size * 2);
    upSampledBuffer.dataR = upSampledBuffer.dataL + upBlockSize;
}

void Preamp::setBias(float bias, int tube_index) {
    ZoneScoped;
    // @TODO: add smoothing to avoid clicks
    float *selected_stage_bias = nullptr;
    switch (tube_index) {
        case 0: { selected_stage_bias = stage1_bias; break; }
        case 1: { selected_stage_bias = stage2_bias; break; }
        case 2: { selected_stage_bias = stage3_bias; break; }
        case 3: { selected_stage_bias = stage4_bias; break; }
        case 4: { selected_stage_bias = stage5_bias; break; }
        default: { assert(false && "setBias: wrong tube_index"); }
    }

    local_const float positiveLinRange = 0.2f;

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

static void gridConduction(Slice buffer, float ratio) {
    ZoneScoped;

    // ratio 8 ou 4

    constexpr float threshold = 1.5f;
    float inv_ratio = 1.0f/ratio;
    float knee = ratio / 4.0f;
    float inv_2_knee = 1.0f/(2.0f*knee);
    // @TODO: pour le bias, puis utiliser un Onepole pour smooth l'offset calculé sur ~10ms
    // refaire une fonction de calcul d'un seul echantillon pour les onepole

    float *buffers[2] = {buffer.dataL, buffer.dataR};
    u32 nChannels = buffer.dataR ? 2 : 1;

    for (u32 channelIndex = 0; channelIndex < nChannels; channelIndex++) {

        for (u32 index = 0; index < buffer.size; index++) {
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

static void tubeSim(Slice buffer, float bias[2]) {
    ZoneScoped;
    
    float *buffers[2] = {buffer.dataL, buffer.dataR};
    u32 nChannels = buffer.dataR ? 2 : 1;
    
    for (u32 channelIndex = 0; channelIndex < nChannels; channelIndex++) {
        for (u32 index = 0; index < buffer.size; index++) {
            float sample = buffers[channelIndex][index];
            
            sample *= -1.0f;
            sample += bias[0];
    
            constexpr float positiveLinRange = 0.5f;
            constexpr float negClipPoint = 3.0f;
                
            if (sample > positiveLinRange) {
                // sample = tanh(sample - positiveLinRange) + positiveLinRange;
                // sample = 1.0f - expf(positiveLinRange - sample) + positiveLinRange;
                float temp = (sample-positiveLinRange) * 0.5f + 1.0f;
                sample = 1.0f - 1.0f/(temp*temp) + positiveLinRange;
            }

            buffers[channelIndex][index] = (sample - bias[1]);
        }
    }
}


void Preamp::process(Slice buffer) {
    ZoneScopedN("Preamp");
    
    Slice upBuffer = upSampledBuffer;
    upBuffer.size = buffer.size*PREAMP_UP_SAMPLE_FACTOR;
    if (buffer.dataR == nullptr) { upBuffer.dataR = nullptr; }

    {
        ZoneScopedN("Upsampling");

        memsetZeroFloat(upBuffer.dataL, upBuffer.size);
        for (u32 i = 0; i < buffer.size; i++) {
            upBuffer.dataL[PREAMP_UP_SAMPLE_FACTOR*i] = buffer.dataL[i];
        }
    
        if (buffer.dataR) {
            memsetZeroFloat(upBuffer.dataR, upBuffer.size);
            for (u32 i = 0; i < buffer.size; i++) {
                upBuffer.dataR[PREAMP_UP_SAMPLE_FACTOR*i] = buffer.dataR[i];
            }
        }
    
        overSampler.upSampleFilter1.process(upBuffer);
        overSampler.upSampleFilter2.process(upBuffer);
        applyGainLinear(upBuffer, PREAMP_UP_SAMPLE_FACTOR);
    }


    {
        ZoneScopedN("GainsStages");
        
        // ------------ Stage 1 ------------
        gridConduction(upBuffer, 2.0f);        
        cathodeBypassFilter1.process(upBuffer);        
        tubeSim(upBuffer, stage1_bias);

        inputFilter.process(upBuffer);
        stage1LP.process(upBuffer);

        stage1Gain.applySmoothGainLinear(upBuffer);
        inputMudFilter.process(upBuffer);
        midBoost.process(upBuffer);

        if (bright) {
            brightCapFilter.process(upBuffer);
        }


        // ------------ Stage 2 ------------
        gridConduction(upBuffer, 4.0f);
        if (channel != 1) { 
            cathodeBypassFilter2.process(upBuffer); 
        }
        tubeSim(upBuffer, stage2_bias);
        
        couplingFilter2.process(upBuffer);
        stage2LP.process(upBuffer);
    
        if (channel == 1) {
            goto gain_stages_end_of_scope;
        }

        stage2Gain.applySmoothGainLinear(upBuffer);


        // ------------ Stage 3 ------------
        gridConduction(upBuffer, 4.0f);
        if (channel != 2) { cathodeBypassFilter3.process(upBuffer); }
        tubeSim(upBuffer, stage3_bias);
        
        couplingFilter3.process(upBuffer);
        stage3LP.process(upBuffer);

        if (channel == 2) {
            goto gain_stages_end_of_scope;
        }

        stage3Gain.applySmoothGainLinear(upBuffer);


        // ------------ Stage 4 ------------
        gridConduction(upBuffer, 4.0f);
        if (channel != 3) { cathodeBypassFilter4.process(upBuffer); }
        tubeSim(upBuffer, stage4_bias);
        
        couplingFilter4.process(upBuffer);
        stage4LP.process(upBuffer);

        if (channel == 3) {
            goto gain_stages_end_of_scope;
        }

        stage4Gain.applySmoothGainLinear(upBuffer);


        // ------------ Stage 5 ------------
        gridConduction(upBuffer, 8.0f);
        tubeSim(upBuffer, stage5_bias);
        
        couplingFilter5.process(upBuffer);
        stage5LP.process(upBuffer);

        gain_stages_end_of_scope: {}
    }

    volume.applySmoothGainLinear(upBuffer);


    {
        ZoneScopedN("DownSampling");        
        overSampler.downSampleFilter1.process(upBuffer);
        overSampler.downSampleFilter2.process(upBuffer);
    
        for (u32 i = 0; i < buffer.size; i++) {
            buffer.dataL[i] = upBuffer.dataL[i*PREAMP_UP_SAMPLE_FACTOR];
        }
        if (buffer.dataR) {
            for (u32 i = 0; i < buffer.size; i++) {
                buffer.dataR[i] = upBuffer.dataR[i*PREAMP_UP_SAMPLE_FACTOR];
            }
        }
    }
}
