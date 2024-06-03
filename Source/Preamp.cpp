/*
  ==============================================================================
    Author:  Benjamin Quiedeville 
  ==============================================================================
*/

#include "Preamp.h"
#include "data/waveshape_table.inc"

static const Sample STAGE_0_GAIN = (Sample)dbtoa(35.0) * 0.2f;
static const Sample STAGE_1_GAIN = (Sample)dbtoa(35.0) * 0.9f;
static const Sample STAGE_2_GAIN = (Sample)dbtoa(35.0) * 0.5f;
static const Sample STAGE_3_GAIN = (Sample)dbtoa(35.0) * 0.5f;
static const Sample STAGE_4_GAIN = (Sample)dbtoa(35.0) * 0.25f;

static const Sample OUTPUT_ATTENUATION = (Sample)dbtoa(-32.0);
static const Sample STAGE_ONE_COMPENSATION = (Sample)dbtoa(21.0);
static const Sample STAGE_TWO_COMPENSATION = (Sample)dbtoa(3.0);



Preamp::Preamp() {
    overSampler = new OverSampler();
}

Preamp::~Preamp() {
    delete overSampler; 

    if (upSampledBlockL) { free(upSampledBlockL); }
    if (upSampledBlockR) { free(upSampledBlockR); }
}

void Preamp::prepareToPlay(double samplerate, u32 blockSize) {

    preGain.init(0.0);
    postGain.init(dbtoa(-12.0));
    
    inputFilter.prepareToPlay();
    couplingFilter1.prepareToPlay();
    couplingFilter2.prepareToPlay();
    couplingFilter3.prepareToPlay();
    couplingFilter4.prepareToPlay();
    
    stageOutputFilter1.prepareToPlay();
    stageOutputFilter2.prepareToPlay();
    stageOutputFilter3.prepareToPlay();
    stageOutputFilter4.prepareToPlay();
    

    inputFilter.setCoefficients(300.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    brightCapFilter.setCoefficients(750.0, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    
    couplingFilter1.setCoefficients(500.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter2.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter3.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter4.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    stageOutputFilter1.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter2.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter3.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter4.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    cathodeBypassFilter1.setCoefficients(250.0, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter2.setCoefficients(200.0, -2.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter3.setCoefficients(250.0, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter4.setCoefficients(200.0, -3.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    overSampler->upSampleFilter1.prepareToPlay();
    overSampler->upSampleFilter2.prepareToPlay();
    overSampler->downSampleFilter1.prepareToPlay();
    overSampler->downSampleFilter2.prepareToPlay();

    // earlevel.com/main/2016/09/29/cascading-filters
    overSampler->upSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, atodb(PREAMP_UP_SAMPLE_FACTOR), samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler->upSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, atodb(PREAMP_UP_SAMPLE_FACTOR), samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler->downSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler->downSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    if (upSampledBlockL) {
        upSampledBlockL = (Sample *)realloc(upSampledBlockL, blockSize * PREAMP_UP_SAMPLE_FACTOR * sizeof(Sample));
    } else {
        upSampledBlockL = (Sample *)calloc(blockSize * PREAMP_UP_SAMPLE_FACTOR, sizeof(Sample));
    }
    
    if (upSampledBlockR) {
        upSampledBlockR = (Sample *)realloc(upSampledBlockR, blockSize * PREAMP_UP_SAMPLE_FACTOR * sizeof(Sample));
    } else {
        upSampledBlockR = (Sample *)calloc(blockSize * PREAMP_UP_SAMPLE_FACTOR, sizeof(Sample));
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
    
        if (sample < -1.3f) { sample = -1.3f; }
        buffer[i] = -sample;
    }
}

static inline void hardClipping(Sample *buffer, u32 nSamples) {

    if (!buffer) { return; }

    for (u32 i = 0; i < nSamples; i++) {

        Sample sample = buffer[i];

        buffer[i] = sample > 1.5f ? 1.5f : sample < -1.5f ? -1.5f : sample;
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

void Preamp::processGainStagesMono(Sample *buffer, u32 nSamples) {
    
    u32 index = 0;
    
    // Stage 0
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= STAGE_0_GAIN;
    }
            
    waveShaping2(buffer, nSamples);
    cathodeBypassFilter1.processLeft(buffer, nSamples);
    
    inputFilter.processHighpassLeft(buffer, nSamples);
    stageOutputFilter1.processLowpassLeft(buffer, nSamples);
    
    
    // Stage 1
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= (Sample)preGain.nextValue() * STAGE_1_GAIN;
    }

    brightCapFilter.processLeft(buffer, nSamples);

    waveShaping2(buffer, nSamples);
    couplingFilter1.processHighpassLeft(buffer, nSamples);

    if (channel == 1) {
        for (index = 0; index < nSamples; index++) {
            buffer[index] *= STAGE_ONE_COMPENSATION;
        }
        return;
    }
    
    
    // Stage 2
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= STAGE_2_GAIN;
    }

    waveShaping2(buffer, nSamples);
    cathodeBypassFilter2.processLeft(buffer, nSamples);
    couplingFilter2.processHighpassLeft(buffer, nSamples);

    if (channel == 2) {
        for (index = 0; index < nSamples; index++) {
            buffer[index] *= -STAGE_TWO_COMPENSATION;
        }
        return;
    }

    stageOutputFilter2.processLowpassLeft(buffer, nSamples);

    
    // Stage 3
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= STAGE_3_GAIN;
    }
    
    waveShaping2(buffer, nSamples);
    cathodeBypassFilter3.processLeft(buffer, nSamples);
    couplingFilter3.processHighpassLeft(buffer, nSamples);

    if (channel == 3) {
        return;        
    }

    stageOutputFilter3.processLowpassLeft(buffer, nSamples);
    
    
    // Stage 4
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= STAGE_4_GAIN;
    }
    
    waveShaping2(buffer, nSamples);
    cathodeBypassFilter4.processLeft(buffer, nSamples);
    couplingFilter4.processHighpassLeft(buffer, nSamples);

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= -1.0f;
    }
}

void Preamp::processGainStagesStereo(Sample *bufferL, Sample *bufferR, u32 nSamples) {
    
    u32 index = 0;
    
    // Stage 0
    for (index = 0; index < nSamples; index++) {
        bufferL[index] *= STAGE_0_GAIN;
        bufferR[index] *= STAGE_0_GAIN;
    }
            
    waveShaping2(bufferL, nSamples);
    waveShaping2(bufferR, nSamples);
    cathodeBypassFilter1.processStereo(bufferL, bufferR, nSamples);
    
    inputFilter.processHighpassStereo(bufferL, bufferR, nSamples);
    stageOutputFilter1.processLowpassStereo(bufferL, bufferR, nSamples);
    
    
    // Stage 1
    for (index = 0; index < nSamples; index++) {
        Sample preGainValue = (Sample)preGain.nextValue() * STAGE_1_GAIN; 
        
        bufferL[index] *= preGainValue;
        bufferR[index] *= preGainValue;
    }

    brightCapFilter.processStereo(bufferL, bufferR, nSamples);

    waveShaping2(bufferL, nSamples);
    waveShaping2(bufferR, nSamples);
    couplingFilter1.processHighpassStereo(bufferL, bufferR, nSamples);

    if (channel == 1) {
        for (index = 0; index < nSamples; index++) {
            bufferL[index] *= STAGE_ONE_COMPENSATION;
            bufferR[index] *= STAGE_ONE_COMPENSATION;
        }
        return;
    }
    
    
    // Stage 2
    for (index = 0; index < nSamples; index++) {
        bufferL[index] *= STAGE_2_GAIN;
        bufferR[index] *= STAGE_2_GAIN;
    }

    waveShaping2(bufferL, nSamples);
    waveShaping2(bufferR, nSamples);
    cathodeBypassFilter2.processStereo(bufferL, bufferR, nSamples);
    couplingFilter2.processHighpassStereo(bufferL, bufferR, nSamples);

    if (channel == 2) {
        for (index = 0; index < nSamples; index++) {
            bufferL[index] *= -STAGE_TWO_COMPENSATION;
            bufferR[index] *= -STAGE_TWO_COMPENSATION;
        }
        return;
    }

    stageOutputFilter2.processLowpassStereo(bufferL, bufferR, nSamples);

    
    // Stage 3
    for (index = 0; index < nSamples; index++) {
        bufferL[index] *= STAGE_3_GAIN;
        bufferR[index] *= STAGE_3_GAIN;
    }
    
    waveShaping2(bufferL, nSamples);
    waveShaping2(bufferR, nSamples);
    cathodeBypassFilter3.processStereo(bufferL, bufferR, nSamples);
    couplingFilter3.processHighpassStereo(bufferL, bufferR, nSamples);

    if (channel == 3) {
        return;        
    }

    stageOutputFilter3.processLowpassStereo(bufferL, bufferR, nSamples);
    
    
    // Stage 4
    for (index = 0; index < nSamples; index++) {
        bufferL[index] *= STAGE_4_GAIN;
        bufferR[index] *= STAGE_4_GAIN;
    }
    
    waveShaping2(bufferL, nSamples);
    waveShaping2(bufferR, nSamples);
    cathodeBypassFilter4.processStereo(bufferL, bufferR, nSamples);
    couplingFilter4.processHighpassStereo(bufferL, bufferR, nSamples);

    for (index = 0; index < nSamples; index++) {
        bufferL[index] *= -1.0f;
        bufferR[index] *= -1.0f;
    }
}


void Preamp::processStereo(Sample *bufferL, Sample *bufferR, u32 nSamples) {
    
    u32 blockSize = nSamples*PREAMP_UP_SAMPLE_FACTOR;        
    
    if (bufferR) {  
        // if bufferR is not nullptr : process in stereo
        // upsampling
        memset(upSampledBlockL, 0, blockSize * sizeof(Sample));
        memset(upSampledBlockR, 0, blockSize * sizeof(Sample));
        for (u32 i = 0; i < nSamples; i++) {
            upSampledBlockL[PREAMP_UP_SAMPLE_FACTOR*i] = bufferL[i];
            upSampledBlockR[PREAMP_UP_SAMPLE_FACTOR*i] = bufferR[i];
        }

        overSampler->upSampleFilter1.processStereo(upSampledBlockL, upSampledBlockR, blockSize);    
        overSampler->upSampleFilter2.processStereo(upSampledBlockL, upSampledBlockR, blockSize);    
    
        //processing
        processGainStagesStereo(upSampledBlockL, upSampledBlockR, blockSize);

        for (u32 index = 0; index < blockSize; index++) {
            Sample postGainValue = OUTPUT_ATTENUATION * (Sample)dbtoa(postGain.nextValue());
            
            upSampledBlockL[index] *= postGainValue;
            upSampledBlockR[index] *= postGainValue;
        }
            
        // downsampling
        assert(blockSize == nSamples*PREAMP_UP_SAMPLE_FACTOR);
    
        overSampler->downSampleFilter1.processStereo(upSampledBlockL, upSampledBlockR, blockSize);
        overSampler->downSampleFilter2.processStereo(upSampledBlockL, upSampledBlockR, blockSize);
    
        for (u32 i = 0; i < nSamples; i++) {
            bufferL[i] = upSampledBlockL[i*PREAMP_UP_SAMPLE_FACTOR];
            bufferR[i] = upSampledBlockR[i*PREAMP_UP_SAMPLE_FACTOR];
        }
    
    } else {
        // otherwise : process in mono
        // upsampling
        memset(upSampledBlockL, 0, blockSize * sizeof(Sample));
        for (u32 i = 0; i < nSamples; i++) {
            upSampledBlockL[PREAMP_UP_SAMPLE_FACTOR*i] = bufferL[i];
        }

        overSampler->upSampleFilter1.processLeft(upSampledBlockL, blockSize);    
        overSampler->upSampleFilter2.processLeft(upSampledBlockL, blockSize);    
    
        //processing
        processGainStagesMono(upSampledBlockL, blockSize);

        for (u32 index = 0; index < blockSize; index++) {
            Sample postGainValue = OUTPUT_ATTENUATION * (Sample)dbtoa(postGain.nextValue());
            
            upSampledBlockL[index] *= postGainValue;
        }
            
        // downsampling
        assert(blockSize == nSamples*PREAMP_UP_SAMPLE_FACTOR);
    
        overSampler->downSampleFilter1.processLeft(upSampledBlockL, blockSize);
        overSampler->downSampleFilter2.processLeft(upSampledBlockL, blockSize);
    
        for (u32 i = 0; i < nSamples; i++) {
            bufferL[i] = upSampledBlockL[i*PREAMP_UP_SAMPLE_FACTOR];
        }
    }
}