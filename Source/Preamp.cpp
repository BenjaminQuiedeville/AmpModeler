 /*
  ==============================================================================
    Author:  Benjamin Quiedeville 
  ==============================================================================
*/

#include "Preamp.h"
#include "data/waveshape_table.inc"

Preamp::Preamp() {
}

Preamp::~Preamp() {

    if (upBufferL) { free(upBufferL); }
}

void Preamp::prepareToPlay(double samplerate, u32 blockSize) {

    preGain.init(0.0);
    postGain.init(dbtoa(-12.0));
    
    inputFilter.prepareToPlay();
    couplingFilter1.prepareToPlay();
    couplingFilter2.prepareToPlay();
    couplingFilter3.prepareToPlay();
    couplingFilter4.prepareToPlay();
    
    stageOutputFilter0.prepareToPlay();
    stageOutputFilter2.prepareToPlay();
    stageOutputFilter3.prepareToPlay();
    stageOutputFilter4.prepareToPlay();
    

    inputFilter.setCoefficients(300.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    brightCapFilter.setCoefficients(750.0, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    
    couplingFilter1.setCoefficients(500.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter2.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter3.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter4.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    stageOutputFilter0.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter2.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter3.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter4.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    cathodeBypassFilter0.setCoefficients(250.0, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter2.setCoefficients(200.0, -2.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter3.setCoefficients(250.0, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter4.setCoefficients(200.0, -3.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    overSampler.upSampleFilter1.prepareToPlay();
    overSampler.upSampleFilter2.prepareToPlay();
    overSampler.downSampleFilter1.prepareToPlay();
    overSampler.downSampleFilter2.prepareToPlay();

    // earlevel.com/main/2016/09/29/cascading-filters
    overSampler.upSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler.upSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler.downSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler.downSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);


    if (upBufferL) {
        upBufferL = (Sample *)realloc(upBufferL, blockSize * PREAMP_UP_SAMPLE_FACTOR * sizeof(Sample) * 2);
        upBufferR = upBufferL + blockSize * PREAMP_UP_SAMPLE_FACTOR;
    } else {
        upBufferL = (Sample *)calloc(blockSize * PREAMP_UP_SAMPLE_FACTOR * 2,  sizeof(Sample));
        upBufferR = upBufferL + blockSize * PREAMP_UP_SAMPLE_FACTOR;
    }
}

static inline void waveShaping2(Sample *buffer, u32 nSamples) {

    if (!buffer) { return; }
    
    auto cubicClip = [](Sample x) { return x < -1.0f ? -2.0f/3.0f : x - 1.0f/3.0f * x*x*x; };
    
    for (u32 i = 0; i < nSamples; i++) {
    
        Sample sample = buffer[i];
        sample = sample > 0.0f ? std::tanh(sample) : 3.0f * cubicClip(1.0f/3.0f * sample);
    
        buffer[i] = -sample;
    }
}

static inline void waveShaping(Sample *buffer, u32 nSamples) {

    if (!buffer) { return; }

    for (u32 i = 0; i < nSamples; i++) {
        
        Sample sample = buffer[i];
        if (sample > 0.0f) { 
            sample = 1/juce::MathConstants<Sample>::pi * 2.0f 
                   * std::atan(sample * juce::MathConstants<Sample>::pi * 0.5f); 
        }
    
        if (sample < -2.0f) { sample = -2.0f; }
        buffer[i] = -sample;
    }
}


static void tube_sim(Sample *buffer, u32 nSamples, Sample gain) {
        
    if (!buffer) { return; }
        
    const float gridCondThresh = 1.0f;
    const float gridCondRatio  = 4.0f;
    const float gridCondKnee   = 0.01f;
    
    for (u32 index = 0; index < nSamples; index++) {
        Sample sample = buffer[index];
        
        if (2.0f * (sample - gridCondThresh) > gridCondKnee) {
            sample = gridCondThresh + (sample - gridCondThresh)/gridCondRatio;
        } else if (2.0 * abs(sample - gridCondThresh) <= gridCondKnee) {
            sample += ((1.0f/gridCondRatio - 1.0f) * powf(sample - gridCondThresh + gridCondKnee * 0.5f, 2))
                        /(2.0f * gridCondKnee);
        }

        sample *= -1.0f;  
        if (sample > 0.0f) {
            sample = tanh(sample);
        } else if (sample < -3.0f) {
            // sample = (tanh(sample + 2.0f) - 2.0f) * gain;
            sample = -3.0f;
        }

        buffer[index] = sample * gain;    
    }
}

static inline void tableWaveshape(Sample *buffer, u32 nSamples) {

    if (!buffer) { return; }

    for (u32 i = 0; i < nSamples; i++) {
    
        Sample sample = buffer[i];
        Sample normalizedPosition = scale_linear(sample, table_min, table_max, 0.0f, 1.0f);
        
        int tableIndex = (int)(normalizedPosition * WAVESHAPE_TABLE_SIZE);
        float interpCoeff = normalizedPosition * WAVESHAPE_TABLE_SIZE - (float)tableIndex;
        
        if (tableIndex >= WAVESHAPE_TABLE_SIZE) {
            buffer[i] = waveshaping_table[WAVESHAPE_TABLE_SIZE-1];
            continue;
        }
        
        if (tableIndex < 0) {
            buffer[i] = waveshaping_table[0];
            continue;
        }
        
        // interpolation Lagrange 3rd order
        
        u32 index2 = tableIndex + 1;
        u32 index3 = tableIndex + 2;
        u32 index4 = tableIndex + 4;
        
        if (index4 > WAVESHAPE_TABLE_SIZE) {
            buffer[i] = waveshaping_table[tableIndex];
            continue;
        }
        
        float value1 = waveshaping_table[tableIndex];
        float value2 = waveshaping_table[index2];
        float value3 = waveshaping_table[index3];
        float value4 = waveshaping_table[index4];
        
        float d1 = interpCoeff - 1.0f;
        float d2 = interpCoeff - 2.0f;
        float d3 = interpCoeff - 3.0f;
        
        float oneSixth = 1.0f/6.0f;
        
        float c1 = -d1 * d2 * d3 * oneSixth;
        float c2 = d2 * d3 * 0.5f;
        float c3 = -d1 * d3 * 0.5f;
        float c4 = d1 * d2 * oneSixth;

        buffer[i] = value1 * c1 + interpCoeff * (value2 * c2 + value3 * c3 + value4 * c4);
        
    }
}

void Preamp::process(Sample *bufferL, Sample *bufferR, u32 nSamples) {

    static const Sample OUTPUT_ATTENUATION = (Sample)dbtoa(-32.0);

    u32 upNumSamples = nSamples*PREAMP_UP_SAMPLE_FACTOR;        
    
    // upsampling
    memset(upBufferL, 0, upNumSamples * sizeof(Sample));
    for (u32 i = 0; i < nSamples; i++) {
        upBufferL[PREAMP_UP_SAMPLE_FACTOR*i] = bufferL[i];
    }

    if (bufferR) {  
        memset(upBufferR, 0, upNumSamples * sizeof(Sample));
        for (u32 i = 0; i < nSamples; i++) {
            upBufferR[PREAMP_UP_SAMPLE_FACTOR*i] = bufferR[i];
        }    
    }

    overSampler.upSampleFilter1.process(upBufferL, upBufferR, upNumSamples);    
    overSampler.upSampleFilter2.process(upBufferL, upBufferR, upNumSamples);    
    
    //processing the gain stages
    {
        static const Sample STAGE_0_GAIN = (Sample)dbtoa(35.0) * 0.2f;
        static const Sample STAGE_1_GAIN = (Sample)dbtoa(35.0) * 0.9f;
        static const Sample STAGE_2_GAIN = (Sample)dbtoa(35.0) * 0.5f;
        static const Sample STAGE_3_GAIN = (Sample)dbtoa(35.0) * 0.5f;
        static const Sample STAGE_4_GAIN = (Sample)dbtoa(35.0) * 0.25f;
        
        static const Sample STAGE_0_BIAS = 0.0;
        static const Sample STAGE_1_BIAS = 0.0;
        static const Sample STAGE_2_BIAS = 0.0;
        static const Sample STAGE_3_BIAS = 0.0;
        static const Sample STAGE_4_BIAS = 0.0;

        static const Sample STAGE_ONE_COMPENSATION = (Sample)dbtoa(21.0);
        static const Sample STAGE_TWO_COMPENSATION = (Sample)dbtoa(3.0);
     
        u32 index = 0;
        
        // Stage 0
        tube_sim(upBufferL, upNumSamples, STAGE_0_GAIN);
        tube_sim(upBufferR, upNumSamples, STAGE_0_GAIN);
        cathodeBypassFilter0.process(upBufferL, upBufferR, upNumSamples);
        
        inputFilter.processHighpass(upBufferL, upBufferR, upNumSamples);
        stageOutputFilter0.processLowpass(upBufferL, upBufferR, upNumSamples);
        
        
        // Stage 1
        if (upBufferR) {
            for (index = 0; index < upNumSamples; index++) {
                Sample preGainValue = (Sample)preGain.nextValue(); 
                upBufferL[index] *= preGainValue;
                upBufferR[index] *= preGainValue;
            }
        } else {
            for (index = 0; index < upNumSamples; index++) {
                Sample preGainValue = (Sample)preGain.nextValue(); 
                upBufferL[index] *= preGainValue;
            }
        }
    
        brightCapFilter.process(upBufferL, upBufferR, upNumSamples);
    
        tube_sim(upBufferL, upNumSamples, STAGE_1_GAIN);
        tube_sim(upBufferR, upNumSamples, STAGE_1_GAIN);
        couplingFilter1.processHighpass(upBufferL, upBufferR, upNumSamples);
    
        if (channel == 1) {
            for (index = 0; index < upNumSamples; index++) {
                upBufferL[index] *= STAGE_ONE_COMPENSATION;
            }
            if (upBufferR) {
                for (index = 0; index < upNumSamples; index++) {
                    upBufferR[index] *= STAGE_ONE_COMPENSATION;
                }
            }
            goto gain_stages_end_of_scope;
        }
        
        
        // Stage 2
        tube_sim(upBufferL, upNumSamples, STAGE_2_GAIN);
        tube_sim(upBufferR, upNumSamples, STAGE_2_GAIN);
        cathodeBypassFilter2.process(upBufferL, upBufferR, upNumSamples);
        couplingFilter2.processHighpass(upBufferL, upBufferR, upNumSamples);
    
        if (channel == 2) {
            for (index = 0; index < upNumSamples; index++) {
                upBufferL[index] *= -STAGE_TWO_COMPENSATION;
            }
            if (upBufferR) {
                for (index = 0; index < upNumSamples; index++) {
                    upBufferR[index] *= -STAGE_TWO_COMPENSATION;
                }
            }
            goto gain_stages_end_of_scope;
        }
    
        stageOutputFilter2.processLowpass(upBufferL, upBufferR, upNumSamples);
    
        
        // Stage 3
        tube_sim(upBufferL, upNumSamples, STAGE_3_GAIN);
        tube_sim(upBufferR, upNumSamples, STAGE_3_GAIN);
        cathodeBypassFilter3.process(upBufferL, upBufferR, upNumSamples);
        couplingFilter3.processHighpass(upBufferL, upBufferR, upNumSamples);
    
        if (channel == 3) {
            goto gain_stages_end_of_scope;        
        }
    
        stageOutputFilter3.processLowpass(upBufferL, upBufferR, upNumSamples);
        
        
        // Stage 4
        tube_sim(upBufferL, upNumSamples, STAGE_4_GAIN);
        tube_sim(upBufferR, upNumSamples, STAGE_4_GAIN);
        cathodeBypassFilter4.process(upBufferL, upBufferR, upNumSamples);
        couplingFilter4.processHighpass(upBufferL, upBufferR, upNumSamples);
    
        for (index = 0; index < upNumSamples; index++) {
            upBufferL[index] *= -1.0f;
        }
        if (upBufferR) {
            for (index = 0; index < upNumSamples; index++) {
                upBufferR[index] *= -1.0f;
            }
        }
        
        gain_stages_end_of_scope:;
    }
    
    
    if (bufferR) {
        for (u32 index = 0; index < upNumSamples; index++) {
            Sample postGainValue = OUTPUT_ATTENUATION * (Sample)dbtoa(postGain.nextValue());
            
            upBufferL[index] *= postGainValue;
            upBufferR[index] *= postGainValue;
        }
    } else {
        for (u32 index = 0; index < upNumSamples; index++) {
            Sample postGainValue = OUTPUT_ATTENUATION * (Sample)dbtoa(postGain.nextValue());
                        
            upBufferL[index] *= postGainValue;
        }
    }
            
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
