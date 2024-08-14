/*
==============================================================================
    Author:  Benjamin Quiedeville
==============================================================================
*/

#ifndef IR_LOADER_H
#define IR_LOADER_H


#include "common.h"
#include "pffft/pffft.h"

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

    IRLoader();
    ~IRLoader();

    void init(double samplerate, size_t blockSize);

    void deallocateFFTEngine();
    void reallocFFTEngine(u64 newSize);
    IRLoaderError loadIR(bool initIR);
    void prepareConvolution(float* irPtr, size_t irSize);
    void process(float *bufferL, float *bufferR, size_t nSamples);

    PFFFT_Setup *fftEngine = nullptr;
    size_t fftSize = 16384;
    
    size_t blockSize = 0;
    size_t convolutionResultSize = 0;

    bool bypass = false;
    bool updateIR = false;
    bool defaultIR = true;
    float *irBuffer = nullptr;
    size_t irBufferSize;

    double samplerate = 0.0;

    int overlapAddIndex = 0;
    
    juce::File irFile;
    juce::Array<juce::File> directoryWavFiles;
    int indexOfCurrentFile = 0;

    float *inputBufferPaddedL = nullptr;
    float *inputBufferPaddedR = nullptr;
    float *inputDftBufferL = nullptr;
    float *inputDftBufferR = nullptr;
    float *irDftBuffer = nullptr;
    float *convolutionResultBufferL = nullptr;
    float *convolutionResultBufferR = nullptr;
    float *convolutionResultDftBufferL = nullptr;
    float *convolutionResultDftBufferR = nullptr;
    float *overlapAddBufferL = nullptr;
    float *overlapAddBufferR = nullptr;
    float *fftWorkBuffer = nullptr;
};

static size_t parseWavFile(const std::string& filepath, float **buffer);

#endif // IR_LOADER_H

/*

FFT::TimeVector *vector 

vector

*/