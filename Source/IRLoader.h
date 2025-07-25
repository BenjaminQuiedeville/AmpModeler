/*
==============================================================================
    Author:  Benjamin Quiedeville
==============================================================================
*/

#ifndef IR_LOADER_H
#define IR_LOADER_H

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "common.h"
#include <pffft/pffft.h>

/*

à l'initialisation ouvrir l'IR de base (charger un buffer temporaire et calculer la dft)
libérer le buffer temporaire

au chargement manuel : ouvrir avec un juce::File dans un juce::MemoryBlock
charger dans un buffer temporaire et calculer la dft
libérer le buffer temporaire

*/

enum IRLoaderError {
    OK, 
    Error
};

struct IRLoader {

    ~IRLoader();

    void init(float samplerate, size_t blockSize);

    void deallocateFFTEngine();
    // void reallocFFTEngine(u64 newSize);
    IRLoaderError loadIR();
    void prepareConvolution(float* irPtr, size_t irSize);
    void process(float *bufferL, float *bufferR, size_t nSamples);

    PFFFT_Setup *fftSetup = nullptr;
    float *fftSignalsMemory = nullptr;
    float **ptrBuffersMemory = nullptr;
    
    float *fftTimeInputBufferLeft = nullptr;
    float *fftTimeInputBufferRight = nullptr;
    float *fftTimeOutputBuffer = nullptr;
    float **irDftBuffers = nullptr;
    float **FDLLeft = nullptr;
    float **FDLRight = nullptr;
    float *convolutionDftResult = nullptr;

    u32 numIRParts = 0;
    u64 fftSize = 16384;
    u64 dftSize = 0;
    u64 blockSize = 0;

    bool active = true;
    bool updateIR = false;
    bool defaultIR = true;
    float *irBuffer = nullptr;
    u32 irBufferSize = 0;
    float samplerate = 0.0;
    
    juce::File irFile;
    juce::Array<juce::File> directoryWavFiles;
    int indexOfCurrentFile = 0;
};

#endif // IR_LOADER_H

/*

FFT::TimeVector *vector 

vector

*/
