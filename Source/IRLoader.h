/*
==============================================================================

    IRLoader.h
    Created: 22 Apr 2023 5:34:02pm
    Author:  Benjamin

==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "common.h"
#include "SmoothParam.h"
#include "pffft.h"

/*

à l'initialisation ouvrir l'IR de base (charger un buffer temporaire et calculer la dft)
libérer le buffer temporaire

au chargement manuel : ouvrir avec un juce::File dans un juce::MemoryBlock
charger dans un buffer temporaire et calculer la dft
libérer le buffer temporaire


*/

struct IRLoader {

    IRLoader();
    ~IRLoader();

    void init(double samplerate, int blockSize);

    void loadIR();
    void prepareConvolution(const float* irPtr, int irSize);
    void process(float *input, size_t nSamples);

    int blockSize;
    size_t fftSize;
    size_t convolutionResultSize;

    int overlapAddIndex;

    float *inputBufferPadded;
    float *inputDftBuffer;
    float *irDftBuffer;
    float *convolutionResultBuffer;
    float *overlapAddBuffer;

    // PFFFT_Setup *fftSetup;
    
    bool initIR;

    SmoothParam outLevel; 
};
