/*
==============================================================================

    IRLoader.h
    Created: 22 Apr 2023 5:34:02pm
    Author:  Benjamin

==============================================================================
*/

#pragma once

#ifndef JUCE_MODAL_LOOPS_PERMITTED
    #define JUCE_MODAL_LOOPS_PERMITTED 1
#endif

// #include <JuceHeader.h>
#include "common.h"
#include "SmoothParam.h"
#include "pffft/pffft.hpp"

#define FFT_SIZE 16384
using FFT = pffft::FFT<float, FFT_SIZE>;

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
    size_t parseWavFile(const std::string& filepath, float *buffer);

    int blockSize;
    size_t convolutionResultSize;

    int overlapAddIndex;

    FFT::FreqVector inputDftBuffer;
    FFT::FreqVector irDftBuffer;
    FFT::TimeVector inputBufferPadded;
    FFT::TimeVector convolutionResultBuffer;
    FFT::TimeVector overlapAddBuffer;

    FFT *fftEngine;
    
    bool initIR;

    SmoothParam outLevel; 
};
