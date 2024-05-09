/*
  ==============================================================================
    Author:  Benjamin Quiedeville 
  ==============================================================================
*/

#include "Preamp.h"
#include <memory>
#include <assert.h>
#include "data/waveshape_table.inc"

const sample_t STAGE_GAIN =         (sample_t)DB_TO_GAIN(35.0);
const sample_t OUTPUT_ATTENUATION = (sample_t)DB_TO_GAIN(-32.0);
const sample_t INPUT_GAIN = 0.1f;

const sample_t STAGE_ONE_COMPENSATION = (sample_t)DB_TO_GAIN(24.0);
const sample_t STAGE_TWO_COMPENSATION = (sample_t)DB_TO_GAIN(9.0);



void OverSampler::prepareToPlay(double _samplerate) {

    upSampleFilter1.prepareToPlay();
    upSampleFilter2.prepareToPlay();
    downSampleFilter1.prepareToPlay();
    downSampleFilter2.prepareToPlay();

    // earlevel.com/main/2016/09/29/cascading-filters
    upSampleFilter1.setCoefficients(_samplerate/2 * 0.9, 0.54119610, GAIN_TO_DB(PREAMP_UP_SAMPLE_FACTOR), _samplerate*PREAMP_UP_SAMPLE_FACTOR);
    upSampleFilter2.setCoefficients(_samplerate/2 * 0.9, 1.3065630, GAIN_TO_DB(PREAMP_UP_SAMPLE_FACTOR), _samplerate*PREAMP_UP_SAMPLE_FACTOR);
    downSampleFilter1.setCoefficients(_samplerate/2 * 0.9, 0.54119610, 0.0, _samplerate*PREAMP_UP_SAMPLE_FACTOR);
    downSampleFilter2.setCoefficients(_samplerate/2 * 0.9, 1.3065630, 0.0, _samplerate*PREAMP_UP_SAMPLE_FACTOR);

}

void OverSampler::upSample(sample_t *source, sample_t *upSampled, size_t sourceSize, size_t upSampledSize) {

    assert(upSampledSize == sourceSize * PREAMP_UP_SAMPLE_FACTOR);

    memset(upSampled, 0, upSampledSize * sizeof(sample_t));

    for (size_t i = 0; i < sourceSize; i++) {
        upSampled[PREAMP_UP_SAMPLE_FACTOR*i] = source[i];
    }

    upSampleFilter1.processBuffer(upSampled, upSampledSize);    
    upSampleFilter2.processBuffer(upSampled, upSampledSize);    

}

void OverSampler::downSample(sample_t *upSampled, sample_t *dest, size_t upSampledSize, size_t destSize) {

    assert(upSampledSize == destSize*PREAMP_UP_SAMPLE_FACTOR);

    downSampleFilter1.processBuffer(upSampled, upSampledSize);
    downSampleFilter2.processBuffer(upSampled, upSampledSize);

    for (size_t i = 0; i < destSize; i++) {
        dest[i] = upSampled[i*PREAMP_UP_SAMPLE_FACTOR];
    }
}


Preamp::Preamp() {
    
    overSampler = new OverSampler();
}

Preamp::~Preamp() {
    delete overSampler; 

    if (upSampledBlock) { free(upSampledBlock); }

}

void Preamp::prepareToPlay(double _samplerate, int blockSize) {
    samplerate = _samplerate;

    preGain.init(0.0);
    postGain.init(DB_TO_GAIN(-12.0));
    
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
    
    couplingFilter1.setCoefficients(300.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter2.setCoefficients(20.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter3.setCoefficients(20.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    couplingFilter4.setCoefficients(20.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    stageOutputFilter1.setCoefficients(10000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter2.setCoefficients(5000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter3.setCoefficients(5000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    stageOutputFilter4.setCoefficients(5000.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);

    cathodeBypassFilter1.setCoefficients(250.0, 0.7, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter2.setCoefficients(20.0, 0.7, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter3.setCoefficients(250.0, 0.7, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    cathodeBypassFilter4.setCoefficients(20.0, 0.7, -6.0, samplerate*PREAMP_UP_SAMPLE_FACTOR);
    overSampler->prepareToPlay(_samplerate);

    if (upSampledBlock) {
        upSampledBlock = (sample_t *)realloc(upSampledBlock, blockSize * PREAMP_UP_SAMPLE_FACTOR * sizeof(sample_t));
    } else {
        upSampledBlock = (sample_t *)calloc(blockSize * PREAMP_UP_SAMPLE_FACTOR, sizeof(sample_t));
    }

}

static inline void waveShaping(sample_t *buffer, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {
        
        sample_t sample = buffer[i];
        if (sample > 0.0f) { 
            sample = 1/juce::MathConstants<sample_t>::pi * 2.0f 
                   * std::atan(sample * juce::MathConstants<sample_t>::pi * 0.5f); 
        }
    
        if (sample < -1.3f) { sample = -1.3f; }
        buffer[i] = -sample;
    }
}

static inline void tableWaveshape(sample_t *buffer, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {
    
        sample_t sample = buffer[i];
        sample_t normalizedPosition = scale_linear(sample, table_min, table_max, 0.0f, 1.0f);
        
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

void Preamp::processGainStages(sample_t *buffer, size_t nSamples) {
    
    size_t index = 0;
    
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= INPUT_GAIN;
        buffer[index] *= STAGE_GAIN;
    }
            
    tableWaveshape(buffer, nSamples);
    cathodeBypassFilter1.processBuffer(buffer, nSamples);
    
    inputFilter.processBufferHighpass(buffer, nSamples);
    stageOutputFilter1.processBufferLowpass(buffer, nSamples);

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.9f;
        buffer[index] *= (sample_t)preGain.nextValue();
        buffer[index] *= STAGE_GAIN;
    }

    brightCapFilter.processBuffer(buffer, nSamples);

    tableWaveshape(buffer, nSamples);
    couplingFilter1.processBufferHighpass(buffer, nSamples);

    if (channel == 1) {
        for (index = 0; index < nSamples; index++) {
            buffer[index] *= STAGE_ONE_COMPENSATION;
            return;
        }
    }

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.5f;
        buffer[index] *= STAGE_GAIN;
    }

    tableWaveshape(buffer, nSamples);
    cathodeBypassFilter2.processBuffer(buffer, nSamples);
    couplingFilter2.processBufferHighpass(buffer, nSamples);

    if (channel == 2) {
        for (index = 0; index < nSamples; index++) {
            buffer[index] *= STAGE_TWO_COMPENSATION;
            return;
        }        
    }

    stageOutputFilter2.processBufferLowpass(buffer, nSamples);

    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.5f;
        buffer[index] *= STAGE_GAIN;
    }
    
    tableWaveshape(buffer, nSamples);
    cathodeBypassFilter3.processBuffer(buffer, nSamples);
    couplingFilter3.processBufferHighpass(buffer, nSamples);

    if (channel == 3) {
        return;        
    }

    stageOutputFilter3.processBufferLowpass(buffer, nSamples);
    
    for (index = 0; index < nSamples; index++) {
        buffer[index] *= 0.5f;
        buffer[index] *= STAGE_GAIN;
    }
    
    tableWaveshape(buffer, nSamples);
    cathodeBypassFilter4.processBuffer(buffer, nSamples);
    couplingFilter4.processBufferHighpass(buffer, nSamples);
}

void Preamp::process(sample_t *buffer, size_t nSamples) {
    
    size_t blockSize = nSamples*PREAMP_UP_SAMPLE_FACTOR;        

    overSampler->upSample(buffer, upSampledBlock, nSamples, blockSize);

    processGainStages(upSampledBlock, blockSize);
           
    for (size_t index = 0; index < blockSize; index++) {
        upSampledBlock[index] *= OUTPUT_ATTENUATION;
        upSampledBlock[index] *= (sample_t)DB_TO_GAIN(postGain.nextValue());
    }
    
    overSampler->downSample(upSampledBlock, buffer, blockSize, nSamples);
}