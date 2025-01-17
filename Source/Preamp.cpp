 /*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "Preamp.h"
// #include "data/waveshape_table.inc"

Preamp::Preamp() {
}

Preamp::~Preamp() {

    if (upBufferL) { free(upBufferL); }
}

void Preamp::prepareToPlay(double samplerate, u32 blockSize) {

    double upSamplerate = samplerate*PREAMP_UP_SAMPLE_FACTOR;
    u32 upBlockSize = blockSize * PREAMP_UP_SAMPLE_FACTOR;

    preGain.init(0.0);
    postGain.init(dbtoa(-12.0));

    inputMudFilter.prepareToPlay();
    inputMudFilter.setCoefficients(400.0, upSamplerate);

    midBoost.prepareToPlay();
    midBoost.setCoefficients(1000.0, 0.2, 3.0, upSamplerate);

    inputFilter.prepareToPlay();
    couplingFilter1.prepareToPlay();
    couplingFilter2.prepareToPlay();
    couplingFilter3.prepareToPlay();
    couplingFilter4.prepareToPlay();

    stageOutputFilter0.prepareToPlay();
    stageOutputFilter1.prepareToPlay();
    stageOutputFilter2.prepareToPlay();
    stageOutputFilter3.prepareToPlay();
    stageOutputFilter4.prepareToPlay();

    cathodeBypassFilter0.prepareToPlay();
    cathodeBypassFilter1.prepareToPlay();
    cathodeBypassFilter2.prepareToPlay();
    cathodeBypassFilter3.prepareToPlay();
    cathodeBypassFilter4.prepareToPlay();

    overSampler.upSampleFilter1.prepareToPlay();
    overSampler.upSampleFilter2.prepareToPlay();
    overSampler.downSampleFilter1.prepareToPlay();
    overSampler.downSampleFilter2.prepareToPlay();

    // earlevel.com/main/2016/09/29/cascading-filters
    overSampler.upSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, 0.0, upSamplerate);
    overSampler.upSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, 0.0, upSamplerate);
    overSampler.downSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, 0.0, upSamplerate);
    overSampler.downSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, 0.0, upSamplerate);

    if (upBufferL) {
        upBufferL = (float *)realloc(upBufferL, upBlockSize * sizeof(float) * 2);
        upBufferR = upBufferL + upBlockSize;
    } else {
        upBufferL = (float *)calloc(upBlockSize * 2,  sizeof(float));
        upBufferR = upBufferL + upBlockSize;
    }
}

void Preamp::setBias(float bias, int tube_index) {
    // add smoothing
    float *selected_stage_bias = nullptr;
    switch (tube_index) {
        case 0: { selected_stage_bias = stage0_bias; break; }
        case 1: { selected_stage_bias = stage1_bias; break; }
        case 2: { selected_stage_bias = stage2_bias; break; }
        case 3: { selected_stage_bias = stage3_bias; break; }
        case 4: { selected_stage_bias = stage4_bias; break; }
        default: { assert(false && "setBias: wrong tube_index"); }
    }

    static const float pos_clip_point = 0.2f;
    static const float neg_clip_point = 3.0f;
    static const float bias_multiplier = 1.5f;

    bias *= bias_multiplier;

    selected_stage_bias[0] = bias;

    float result = 0.0f;

    if (bias > pos_clip_point) {
        result = tanh(bias - pos_clip_point) + pos_clip_point;
    } else if (bias < neg_clip_point) {
        result = neg_clip_point;
    } else {
        result = bias;
    }

    selected_stage_bias[1] = result;
}

// auto cubicClip = [](float x) { return x < -1.0f ? -2.0f/3.0f : x - 1.0f/3.0f * x*x*x; };

static void tube_sim(float *buffer, u32 nSamples, float gain, float *bias) {
    ZoneScoped;
    
    if (!buffer) { return; }

    static const float gridCondThresh = 1.0f;
    static const float gridCondRatio  = 2.0f;
    // static const float gridCondKnee   = 0.05f;
    static const float gridCondKnee   = gridCondRatio / 4.0f;

    // pour le bias, puis utiliser un Onepole pour smooth l'offset calculé sur ~10ms
    // refaire une fonction de calcul d'un seul echantillon pour les onepole
    // toujours trouver si le cathode bypass doit se faire avant ou apres le tube cutoff

    for (u32 index = 0; index < nSamples; index++) {
        float sample = buffer[index];

        if (2.0f * (sample - gridCondThresh) > gridCondKnee) {
            sample = gridCondThresh + (sample - gridCondThresh)/gridCondRatio;
                    // sample = thresh + sample/ratio - thresh/ratio
                    // sample = thresh*ratio + sample - thresh
                    // sample = sample + thresh*ratio - thresh
        } else if (2.0f * abs(sample - gridCondThresh) <= gridCondKnee) {
            sample += ((1.0f/gridCondRatio - 1.0f) * powf(sample - gridCondThresh + gridCondKnee * 0.5f, 2))
                        /(2.0f * gridCondKnee);
        }

        sample *= -1.0f;
        sample += bias[0];

        static const float pos_clip_point = 0.2f;
        static const float neg_clip_point = 3.0f;

        if (sample > pos_clip_point) {
            sample = tanh(sample - pos_clip_point) + pos_clip_point;
        }
        // else if (sample < neg_clip_point) {
        //     sample = neg_clip_point;
        // }
        else if (sample < 0.0f) {
            sample *= 2.0f/(3.0f*neg_clip_point);
            sample = sample < -1.0f ? -2.0f/3.0f : sample - 1.0f/3.0f * sample*sample*sample;
            sample *= neg_clip_point * 1.5f;
        }


        buffer[index] = (sample - bias[1]) * gain;
    }
}

// static inline void tableWaveshape(float *buffer, u32 nSamples) {

//     if (!buffer) { return; }

//     for (u32 i = 0; i < nSamples; i++) {

//         float sample = buffer[i];
//         float normalizedPosition = scale_linear(sample, table_min, table_max, 0.0f, 1.0f);

//         int tableIndex = (int)(normalizedPosition * WAVESHAPE_TABLE_SIZE);
//         float interpCoeff = normalizedPosition * WAVESHAPE_TABLE_SIZE - (float)tableIndex;

//         if (tableIndex >= WAVESHAPE_TABLE_SIZE) {
//             buffer[i] = waveshaping_table[WAVESHAPE_TABLE_SIZE-1];
//             continue;
//         }

//         if (tableIndex < 0) {
//             buffer[i] = waveshaping_table[0];
//             continue;
//         }

//         // interpolation Lagrange 3rd order

//         u32 index2 = tableIndex + 1;
//         u32 index3 = tableIndex + 2;
//         u32 index4 = tableIndex + 4;

//         if (index4 > WAVESHAPE_TABLE_SIZE) {
//             buffer[i] = waveshaping_table[tableIndex];
//             continue;
//         }

//         float value1 = waveshaping_table[tableIndex];
//         float value2 = waveshaping_table[index2];
//         float value3 = waveshaping_table[index3];
//         float value4 = waveshaping_table[index4];

//         float d1 = interpCoeff - 1.0f;
//         float d2 = interpCoeff - 2.0f;
//         float d3 = interpCoeff - 3.0f;

//         float oneSixth = 1.0f/6.0f;

//         float c1 = -d1 * d2 * d3 * oneSixth;
//         float c2 = d2 * d3 * 0.5f;
//         float c3 = -d1 * d3 * 0.5f;
//         float c4 = d1 * d2 * oneSixth;

//         buffer[i] = value1 * c1 + interpCoeff * (value2 * c2 + value3 * c3 + value4 * c4);

//     }
// }

void Preamp::process(float *bufferL, float *bufferR, u32 nSamples) {
    ZoneScopedN("Preamp");
    
    u32 upNumSamples = nSamples*PREAMP_UP_SAMPLE_FACTOR;

    // upsampling
    memset(upBufferL, 0, upNumSamples * sizeof(float));
    for (u32 i = 0; i < nSamples; i++) {
        upBufferL[PREAMP_UP_SAMPLE_FACTOR*i] = bufferL[i];
    }

    if (bufferR) {
        memset(upBufferR, 0, upNumSamples * sizeof(float));
        for (u32 i = 0; i < nSamples; i++) {
            upBufferR[PREAMP_UP_SAMPLE_FACTOR*i] = bufferR[i];
        }
    }

    overSampler.upSampleFilter1.process(upBufferL, upBufferR, upNumSamples);
    overSampler.upSampleFilter2.process(upBufferL, upBufferR, upNumSamples);

    //processing the gain stages
    {
        ZoneScopedN("GainsStages");
        
        static const float INPUT_GAIN   = (float)dbtoa(0.0);
        static const float STAGE_0_GAIN = (float)dbtoa(40.0);
        static const float STAGE_1_GAIN = (float)dbtoa(40.0) * 0.9f;
        static const float STAGE_2_GAIN = (float)dbtoa(40.0) * 0.6f;
        static const float STAGE_3_GAIN = (float)dbtoa(40.0) * 0.6f;
        static const float STAGE_4_GAIN = 1.0f;

        static const float STAGE1_COMPENSATION = (float)dbtoa(-10.0);
        static const float STAGE2_COMPENSATION = (float)dbtoa(-21.0);
        static const float STAGE3_COMPENSATION = (float)dbtoa(-30.0);
        static const float STAGE4_COMPENSATION = (float)dbtoa(6.0);

        // Input Gain
        // applyGainLinear(INPUT_GAIN, upBufferL, upBufferR, upNumSamples);

        // Stage 0
        tube_sim(upBufferL, upNumSamples, STAGE_0_GAIN, stage0_bias);
        tube_sim(upBufferR, upNumSamples, STAGE_0_GAIN, stage0_bias);
        cathodeBypassFilter0.process(upBufferL, upBufferR, upNumSamples);

        inputFilter.processHighpass(upBufferL, upBufferR, upNumSamples);
        stageOutputFilter0.processLowpass(upBufferL, upBufferR, upNumSamples);


        // Stage 1
        preGain.applySmoothGainLinear(upBufferL, upBufferR, upNumSamples);

        inputMudFilter.processHighpass(upBufferL, upBufferR, upNumSamples);
        midBoost.process(upBufferL, upBufferR, upNumSamples);

        if (bright) {
            brightCapFilter.process(upBufferL, upBufferR, upNumSamples);
        }

        tube_sim(upBufferL, upNumSamples, STAGE_1_GAIN, stage1_bias);
        tube_sim(upBufferR, upNumSamples, STAGE_1_GAIN, stage1_bias);
        couplingFilter1.processHighpass(upBufferL, upBufferR, upNumSamples);

        if (channel == 1) {
            applyGainLinear(STAGE1_COMPENSATION, upBufferL, upBufferR, upNumSamples);
            goto gain_stages_end_of_scope;
        }


        // Stage 2
        tube_sim(upBufferL, upNumSamples, STAGE_2_GAIN, stage2_bias);
        tube_sim(upBufferR, upNumSamples, STAGE_2_GAIN, stage2_bias);
        cathodeBypassFilter2.process(upBufferL, upBufferR, upNumSamples);
        couplingFilter2.processHighpass(upBufferL, upBufferR, upNumSamples);

        if (channel == 2) {
            applyGainLinear(-STAGE2_COMPENSATION, upBufferL, upBufferR, upNumSamples);
            goto gain_stages_end_of_scope;
        }

        stageOutputFilter2.processLowpass(upBufferL, upBufferR, upNumSamples);


        // Stage 3
        tube_sim(upBufferL, upNumSamples, STAGE_3_GAIN, stage3_bias);
        tube_sim(upBufferR, upNumSamples, STAGE_3_GAIN, stage3_bias);
        cathodeBypassFilter3.process(upBufferL, upBufferR, upNumSamples);
        couplingFilter3.processHighpass(upBufferL, upBufferR, upNumSamples);

        if (channel == 3) {
            applyGainLinear(STAGE3_COMPENSATION, upBufferL, upBufferR, upNumSamples);
            goto gain_stages_end_of_scope;
        }

        stageOutputFilter3.processLowpass(upBufferL, upBufferR, upNumSamples);


        // Stage 4
        tube_sim(upBufferL, upNumSamples, STAGE_4_GAIN, stage4_bias);
        tube_sim(upBufferR, upNumSamples, STAGE_4_GAIN, stage4_bias);
        cathodeBypassFilter4.process(upBufferL, upBufferR, upNumSamples);
        couplingFilter4.processHighpass(upBufferL, upBufferR, upNumSamples);
        stageOutputFilter4.processLowpass(upBufferL, upBufferR, upNumSamples);

        applyGainLinear(-STAGE4_COMPENSATION, upBufferL, upBufferR, upNumSamples);

        gain_stages_end_of_scope:;
    }


    postGain.applySmoothGainDeciBels(upBufferL, upBufferR, upNumSamples);

    // downsampling
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
