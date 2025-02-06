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

    void init(double samplerate, size_t blockSize);

    void deallocateFFTEngine();
    IRLoaderError loadIR(u32 whichIR);
    void prepareConvolution();
    void process(float *bufferL, float *bufferR, size_t nSamples);

    PFFFT_Setup *fftSetup = nullptr;
    float *fftSignalsMemory = nullptr;
    
    // float *ir1Memory = nullptr;
    // float *ir2Memory = nullptr;
    
    float **ptrBuffersMemory = nullptr;
    float **irDftBuffers1 = nullptr;
    float **irDftBuffers2 = nullptr;

    float *fftTimeInputBufferLeft = nullptr;
    float *fftTimeInputBufferRight = nullptr;
    float *fftTimeOutputBuffer = nullptr;
    float **FDLLeft = nullptr;
    float **FDLRight = nullptr;
    float *convolutionDftResult = nullptr;

    u32 numIRParts = 0;
    u32 fftSize = 16384;
    u32 dftSize = 0;
    u32 blockSize = 0;

    bool active1 = true;
    bool active2 = false;
    bool updateIR = false;
    
    struct IRData {
        float *buffer = nullptr;
        u32 size = 0;
        juce::File file;
        juce::Array<juce::File> filesArray;
        u32 fileIndex = 0;
        bool defaultIR = true;
    } ir1, ir2;
    
    double samplerate = 0.0;
};

#endif // IR_LOADER_H
