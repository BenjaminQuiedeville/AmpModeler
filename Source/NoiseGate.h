/*
  ==============================================================================

    NoiseGate.h
    Created: 6 May 2023 11:42:39pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
#include "common.h"
#include "SmoothParam.h"

#define GATE_BUFFER_LENGTH_SECONDS 0.1

/*
buffer de taille fixe, 
*/

struct NoiseGate {
    
    NoiseGate() {
        gateGain = new SmoothParamIIR();
    }
    
    ~NoiseGate() {
        delete gateGain;
        
        free(gateBuffer);
    }


    void prepareToPlay(double _samplerate);
    void process(sample_t *input, sample_t *sidechain, size_t nSamples);

    double samplerate;
    sample_t *gateBuffer = nullptr;
    size_t gateBufferLength = 0;
    int32_t gateBufferIndex = 0;
    
    double absoluteSum = 0.0;
    
    SmoothParamIIR *gateGain;
    double threshold = 0.0;
    
    double attackTimeMs = 1.0;
    double releaseTimeMs = 15.0;

};
