/*
  ==============================================================================
    Author:  Benjamin Quiedeville 
  ==============================================================================
*/

#include "Preamp.h"
#include "data/waveshape_table.inc"

const Sample STAGE_GAIN =         (Sample)dbtoa(35.0);
const Sample OUTPUT_ATTENUATION = (Sample)dbtoa(-32.0);
const Sample INPUT_GAIN = 0.2f;

const Sample STAGE_ONE_COMPENSATION = (Sample)dbtoa(21.0);
const Sample STAGE_TWO_COMPENSATION = (Sample)dbtoa(3.0);


Preamp::Preamp() {
    
    overSampler = new OverSampler();
}

Preamp::~Preamp() {
    delete overSampler; 

    if (upSampledBlock) { free(upSampledBlock); }

}

void Preamp::prepareToPlay(double samplerate, int blockSize) {

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

    brightCapFilter.setCoefficients(750.0, 0.4, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    
    couplingFilter1.setCoefficients(500.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter2.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter3.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter4.setCoefficients(10.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    stageOutputFilter1.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter2.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter3.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter4.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    cathodeBypassFilter1.setCoefficients(250.0, 0.25, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter2.setCoefficients(200.0, 0.25, -2.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter3.setCoefficients(250.0, 0.25, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter4.setCoefficients(200.0, 0.25, -3.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    overSampler->upSampleFilter1.prepareToPlay();
    overSampler->upSampleFilter2.prepareToPlay();
    overSampler->downSampleFilter1.prepareToPlay();
    overSampler->downSampleFilter2.prepareToPlay();

    // earlevel.com/main/2016/09/29/cascading-filters
    overSampler->upSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, atodb(PREAMP_UP_SAMPLE_FACTOR), samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler->upSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, atodb(PREAMP_UP_SAMPLE_FACTOR), samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler->downSampleFilter1.setCoefficients(samplerate/2 * 0.9, 0.54119610, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler->downSampleFilter2.setCoefficients(samplerate/2 * 0.9, 1.3065630, 0.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    if (upSampledBlock) {
        upSampledBlock = (Sample *)realloc(upSampledBlock, blockSize * PREAMP_UP_SAMPLE_FACTOR * sizeof(Sample));
    } else {
        upSampledBlock = (Sample *)calloc(blockSize * PREAMP_UP_SAMPLE_FACTOR, sizeof(Sample));
    }

}

static inline void waveShaping2(Sample *buffer, size_t nSamples) {
    
    auto cubicClip = [](Sample x) { return x < -1.0f ? -2.0f/3.0f : x - 1.0f/3.0f * x*x*x; };
    
    for (size_t i = 0; i < nSamples; i++) {
    
        Sample sample = buffer[i];
        sample = sample > 0.0f ? std::tanh(sample) : 3.0f * cubicClip(1.0f/3.0f * sample);
    
        buffer[i] = -sample;
    }
}

static inline void waveShaping(Sample *buffer, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {
        
        Sample sample = buffer[i];
        if (sample > 0.0f) { 
            sample = 1/juce::MathConstants<Sample>::pi * 2.0f 
                   * std::atan(sample * juce::MathConstants<Sample>::pi * 0.5f); 
        }
    
        if (sample < -1.3f) { sample = -1.3f; }
        buffer[i] = -sample;
    }
}

static inline void hardClipping(Sample *buffer, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {

        Sample sample = buffer[i];

        buffer[i] = sample > 1.5f ? 1.5f : sample < -1.5f ? -1.5f : sample;
    }

}

static inline void tableWaveshape(Sample *buffer, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {
    
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
        
        size_t index2 = tableIndex + 1;
        size_t index3 = tableIndex + 2;
        size_t index4 = tableIndex + 4;
        
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

void Preamp::processGainStages(Sample *buffer, size_t nSamples) {
    
    size_t index = 0;
    
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= INPUT_GAIN * STAGE_GAIN;
    }
            
    waveShaping2(buffer, nSamples);
    cathodeBypassFilter1.processBuffer(buffer, nSamples);
    
    inputFilter.processBufferHighpass(buffer, nSamples);
    stageOutputFilter1.processBufferLowpass(buffer, nSamples);

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.9f * (Sample)preGain.nextValue() * STAGE_GAIN;
    }

    brightCapFilter.processBuffer(buffer, nSamples);

    waveShaping2(buffer, nSamples);
    couplingFilter1.processBufferHighpass(buffer, nSamples);

    if (channel == 1) {
        for (index = 0; index < nSamples; index++) {
            buffer[index] *= STAGE_ONE_COMPENSATION;
        }
        return;
    }

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.5f;
        buffer[index] *= STAGE_GAIN;
    }

    waveShaping2(buffer, nSamples);
    cathodeBypassFilter2.processBuffer(buffer, nSamples);
    couplingFilter2.processBufferHighpass(buffer, nSamples);

    if (channel == 2) {
        for (index = 0; index < nSamples; index++) {
            buffer[index] *= -STAGE_TWO_COMPENSATION;
        }        
        return;
    }

    stageOutputFilter2.processBufferLowpass(buffer, nSamples);

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.5f;
        buffer[index] *= STAGE_GAIN;
    }
    
    waveShaping2(buffer, nSamples);
    cathodeBypassFilter3.processBuffer(buffer, nSamples);
    couplingFilter3.processBufferHighpass(buffer, nSamples);

    if (channel == 3) {
        return;        
    }

    stageOutputFilter3.processBufferLowpass(buffer, nSamples);
    
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.2f;
        buffer[index] *= STAGE_GAIN;
    }
    
    waveShaping2(buffer, nSamples);
    cathodeBypassFilter4.processBuffer(buffer, nSamples);
    couplingFilter4.processBufferHighpass(buffer, nSamples);

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= -1.0f;
    }
}

void Preamp::process(Sample *buffer, size_t nSamples) {
    
    size_t blockSize = nSamples*PREAMP_UP_SAMPLE_FACTOR;        
    
    // upsampling
    memset(upSampledBlock, 0, blockSize * sizeof(Sample));

    for (size_t i = 0; i < nSamples; i++) {
        upSampledBlock[PREAMP_UP_SAMPLE_FACTOR*i] = buffer[i];
    }

    overSampler->upSampleFilter1.processBuffer(upSampledBlock, blockSize);    
    overSampler->upSampleFilter2.processBuffer(upSampledBlock, blockSize);    


    //processing
    processGainStages(upSampledBlock, blockSize);
           
    for (size_t index = 0; index < blockSize; index++) {
        upSampledBlock[index] *= OUTPUT_ATTENUATION;
        upSampledBlock[index] *= (Sample)dbtoa(postGain.nextValue());
    }
        
        
    // downsampling
    assert(blockSize == nSamples*PREAMP_UP_SAMPLE_FACTOR);

    overSampler->downSampleFilter1.processBuffer(upSampledBlock, blockSize);
    overSampler->downSampleFilter2.processBuffer(upSampledBlock, blockSize);

    for (size_t i = 0; i < nSamples; i++) {
        buffer[i] = upSampledBlock[i*PREAMP_UP_SAMPLE_FACTOR];
    }
}