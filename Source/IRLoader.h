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
    void reallocFFTEngine(u64 newSize);
    IRLoaderError loadIR();
    void prepareConvolution(float* irPtr, size_t irSize);
    void process(float *bufferL, float *bufferR, size_t nSamples);

    PFFFT_Setup *fftEngine = nullptr;
    size_t fftSize = 16384;
    
    size_t blockSize = 0;
    size_t convolutionResultSize = 0;

    bool active = true;
    bool updateIR = false;
    bool defaultIR = true;
    float *irBuffer = nullptr;
    size_t irBufferSize;

    double samplerate = 0.0;

    int overlapAddIndex = 0;
    
    juce::File irFile;
    juce::Array<juce::File> directoryWavFiles;
    int indexOfCurrentFile = 0;

    float *inputBufferPadded = nullptr;
    float *inputDftBuffer = nullptr;
    float *irDftBuffer = nullptr;
    float *convolutionResultBuffer = nullptr;
    float *convolutionResultDftBuffer = nullptr;
    float *overlapAddBufferL = nullptr;
    float *overlapAddBufferR = nullptr;
    float *fftWorkBuffer = nullptr;
};

#endif // IR_LOADER_H

/*

FFT::TimeVector *vector 

vector

*/
