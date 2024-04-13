/*
==============================================================================
    Author:  Benjamin Quiedeville
==============================================================================
*/

#ifndef IR_LOADER_H
#define IR_LOADER_H


#include "common.h"
#include "pffft/pffft.hpp"

#define FFT_SIZE (1 << 15)
using FFT = pffft::FFT<float, FFT_SIZE>;

/*

à l'initialisation ouvrir l'IR de base (charger un buffer temporaire et calculer la dft)
libérer le buffer temporaire

au chargement manuel : ouvrir avec un juce::File dans un juce::MemoryBlock
charger dans un buffer temporaire et calculer la dft
libérer le buffer temporaire


*/

//@TODO change back to pffft's C version for more flexibility
struct IRLoader {

    IRLoader();
    ~IRLoader();

    void init(double samplerate, size_t blockSize);

    void loadIR(bool initIR);
    void prepareConvolution(float* irPtr, size_t irSize);
    void process(float *input, size_t nSamples);

    size_t blockSize = 0;
    size_t convolutionResultSize = 0;

    bool bypass = false;
    bool updateIR = false;
    float *irBuffer = nullptr;
    size_t irBufferSize;

    double samplerate = 0.0;

    int overlapAddIndex = 0;
    
    juce::File irFile;
    
    FFT::FreqVector inputDftBuffer;
    FFT::FreqVector irDftBuffer;
    FFT::TimeVector inputBufferPadded;
    FFT::TimeVector convolutionResultBuffer;
    FFT::TimeVector overlapAddBuffer;

    FFT *fftEngine;
    
};

static size_t parseWavFile(const std::string& filepath, float **buffer);

#endif // IR_LOADER_H

/*

FFT::TimeVector *vector 

vector

*/