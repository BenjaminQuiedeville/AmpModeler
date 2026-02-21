/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "Preamp.h"

Preamp::~Preamp() {}

void Preamp::prepareToPlay(float samplerate, u32 blockSize) {

    stage1Gain.init(0.0f, blockSize);
    stage2Gain.init(0.0f, blockSize);
    stage3Gain.init(0.0f, blockSize);
    stage4Gain.init(0.0f, blockSize);
    
    volume.init(dbtoa(-60.0f), blockSize);

    inputMudFilter.reset();
    inputMudFilter.makeHighpass(100.0, samplerate);

    midBoost.reset();
    midBoost.setCoefficients(BIQUAD_PEAK, 1000.0, 0.2, 3.0, samplerate);

    inputFilter.reset();
    inputFilter.makeHighpass(100.0f, samplerate);
    
    couplingFilter2.reset();
    couplingFilter3.reset();
    couplingFilter4.reset();
    couplingFilter5.reset();
    couplingFilter2.makeHighpass(15.0, samplerate);
    couplingFilter3.makeHighpass(15.0, samplerate);
    couplingFilter4.makeHighpass(15.0, samplerate);
    couplingFilter5.makeHighpass(15.0, samplerate);

    stage1LP.reset();
    stage2LP.reset();
    stage3LP.reset();
    stage4LP.reset();
    stage5LP.reset();

    cathodeBypassFilter1.reset();
    cathodeBypassFilter2.reset();
    cathodeBypassFilter3.reset();
    cathodeBypassFilter4.reset();
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

    local_const float threshold = 1.5f;
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
                local_const float negClipPoint = 3.0f;
    
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
    
            local_const float positiveLinRange = 0.5f;
                
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
        
    // ------------ Stage 1 ------------
    gridConduction(buffer, 2.0f);        
    cathodeBypassFilter1.process(buffer);        
    tubeSim(buffer, stage1_bias);

    inputFilter.process(buffer);
    stage1LP.process(buffer);

    stage1Gain.applySmoothGainLinear(buffer);
    inputMudFilter.process(buffer);
    midBoost.process(buffer);

    if (bright) {
        brightCapFilter.process(buffer);
    }


    // ------------ Stage 2 ------------
    gridConduction(buffer, 4.0f);
    if (channel != 1) { 
        cathodeBypassFilter2.process(buffer); 
    }
    tubeSim(buffer, stage2_bias);
    
    couplingFilter2.process(buffer);
    stage2LP.process(buffer);

    if (channel == 1) {
        goto gain_stages_end_of_scope;
    }

    stage2Gain.applySmoothGainLinear(buffer);


    // ------------ Stage 3 ------------
    gridConduction(buffer, 4.0f);
    if (channel != 2) { cathodeBypassFilter3.process(buffer); }
    tubeSim(buffer, stage3_bias);
    
    couplingFilter3.process(buffer);
    stage3LP.process(buffer);

    if (channel == 2) {
        goto gain_stages_end_of_scope;
    }

    stage3Gain.applySmoothGainLinear(buffer);


    // ------------ Stage 4 ------------
    gridConduction(buffer, 4.0f);
    if (channel != 3) { cathodeBypassFilter4.process(buffer); }
    tubeSim(buffer, stage4_bias);
    
    couplingFilter4.process(buffer);
    stage4LP.process(buffer);

    if (channel == 3) {
        goto gain_stages_end_of_scope;
    }

    stage4Gain.applySmoothGainLinear(buffer);


    // ------------ Stage 5 ------------
    gridConduction(buffer, 8.0f);
    tubeSim(buffer, stage5_bias);
    
    couplingFilter5.process(buffer);
    stage5LP.process(buffer);

    gain_stages_end_of_scope: {}

    volume.applySmoothGainLinear(buffer);
}
