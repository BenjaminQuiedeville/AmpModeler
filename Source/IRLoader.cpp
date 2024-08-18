/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#define JUCE_MODAL_LOOPS_PERMITTED 1

#include "IRLoader.h"
#include <memory>
#include <math.h>
#include <assert.h>
#include <fstream>
#include <iostream>


constexpr double BITS_24_MAX = (double)((1 << 23) - 1);

#define BASE_IR_SIZE 1323
static float baseIR[BASE_IR_SIZE] = {
#include "data/baseIR.inc"
};


static u64 parseWavFile(const std::string& filepath, float **buffer) {

    char temp[1] = {0};
    char riffString[4];
    s32 ChunkSize;
    char format[4];
    
    char SubChunk1ID[4];
    s32 SubChunk1Size;
    s16 AudioFormat;
    s16 NumChannels;
    s32 samplerate;
    s32 byteRate;
    s16 BlockAlign;
    s16 bitsPerSample;
    
    char SubChunk2ID[4];
    s32 signalSizeBytes;
    
    // FILE *wavFile = fopen(filepath.data(), "rb");
    FILE *wavFile = nullptr;
    fopen_s(&wavFile, filepath.data(), "rb");
    
    if (ferror(wavFile)) {
        fclose(wavFile);
        return 0;
    }

    fread(riffString, 1, 4, wavFile);

    if (memcmp(riffString, "RIFF", 4) != 0) {
        juce::AlertWindow::showMessageBox(
            juce::MessageBoxIconType::WarningIcon, 
            "IR file error",
            "There is an error in the \"RIFF\" header of the .wav file, please choose another file", 
            "Ok");
        
        fclose(wavFile);
        return 0;
    }
    
    // assert(memcmp(riffString, "RIFF", 4) == 0);
    
    fread((char *)(&ChunkSize), 1, 4, wavFile);
    fread(format, 1, 4, wavFile);

    while (temp[0] != 'f') {
        fread(temp, 1, 1, wavFile);
    }

    SubChunk1ID[0] = temp[0];
    temp[0] = 0;

    fread(SubChunk1ID+1, 1, 3, wavFile);

    fread((char *)(&SubChunk1Size), 1, 4, wavFile);
    fread((char *)(&AudioFormat), 1, 2, wavFile);

    fread((char *)(&NumChannels), 1, 2, wavFile);
    fread((char *)(&samplerate), 1, 4, wavFile);
    fread((char *)(&byteRate), 1, 4, wavFile);
    fread((char *)(&BlockAlign), 1, 2, wavFile);
    fread((char *)(&bitsPerSample), 1, 2, wavFile);
    
    // assert(NumChannels == 1);
    

    if (NumChannels != 1) {
        juce::AlertWindow::showMessageBox(
            juce::MessageBoxIconType::WarningIcon, 
            "IR file error",
            "Sorry, I don't support IRs with more than one channels for now, please choose another file", 
            "Ok");
        
        fclose(wavFile);
        return 0;        
    }
    
    // @TODO add non error alertwindow if file samplerate is different from host samplerate 
    assert(byteRate == (samplerate * NumChannels * bitsPerSample/8));
    assert(BlockAlign == (NumChannels * bitsPerSample/8));
    

    while (temp[0] != 'd') {
        fread(temp, 1, 1, wavFile);
    }

    SubChunk2ID[0] = temp[0];
    temp[0] = 0;
    fread(SubChunk2ID+1, 1, 3, wavFile);
    
    // if (SubChunk2ID[0] != 'd' || SubChunk2ID[1] != 'a'
    //  || SubChunk2ID[2] != 't' || SubChunk2ID[3] != 'a')
    

    if (memcmp(SubChunk2ID, "data", 4) != 0) {
        juce::AlertWindow::showMessageBox(
            juce::MessageBoxIconType::WarningIcon, 
            "IR file error",
            "The file contains anoying meta data than I can't decode for now, please choose another file", 
            "Ok");
        
        fclose(wavFile);
        return 0;        
    }
    // assert(memcmp(SubChunk2ID, "data", 4) == 0 && "le fichier contient des métadonnées chiantes");
    

    //if (memcmp(SubChunk2ID, "data", 4) == 0)
    //{
    //    while (temp[0] != 'd') {
    //        fread(temp, 1, 1, wavFile);
    //    }

    //    SubChunk2ID[0] = temp[0];
    //    fread(SubChunk2ID+1, 1, 3, wavFile);

    //}
    
    fread(&signalSizeBytes, 4, 1, wavFile);
    
    u32 sampleSizeBytes = BlockAlign/NumChannels;
    u64 numSamples = signalSizeBytes/sampleSizeBytes;

    // allocate the IR buffer
    if (*buffer != nullptr) {
        *buffer = (float *)realloc(*buffer, numSamples * sizeof(float));
        memset(*buffer, 0, numSamples * sizeof(float));

    } else {
        *buffer = (float *)calloc(numSamples, sizeof(float));
    }


    u32 counter = 0;
    switch (bitsPerSample) {
    case 16: {

        s8 sampleChar[2] = {0x00, 0x00};

        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 2, wavFile);
            
            s32 mask = (s32)(1 << (bitsPerSample - 1));
            s16 sampleInt = *(s16 *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFF;
            }            

            float sample = sampleInt / (float)_I16_MAX;

            (*buffer)[counter] = sample;
            counter++;
        }
    }
    case 24: {
    
        s8 sampleChar[4] = {0x00, 0x00, 0x00, 0x00};
        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 3, wavFile);

            s64 mask = (s64)1 << (bitsPerSample - 1);
            s32 sampleInt = *(s32 *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFF;
            }

            Sample sample = (Sample)sampleInt / (Sample)BITS_24_MAX / 2.0f;

            (*buffer)[counter] = sample;
            counter++;
        }
    }
    case 32: {
        
        s8 sampleChar[4] = {0x00, 0x00, 0x00, 0x00};

        while (!feof(wavFile) && counter < numSamples) {

            fread(sampleChar, 1, 4, wavFile);

            s64 mask = (s64)1 << (bitsPerSample - 1);
            s32 sampleInt = *(s32 *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFFFF;
            }

            float sample = sampleInt / (float)_I32_MAX;

            (*buffer)[counter] = sample;
            counter++;
        }
    }
    }

    float irMax = 0.0f;

    for (u64 i = 0; i < (u64)(numSamples/4); i++) {
        if (abs((*buffer)[i]) > abs(irMax)) {
            irMax = abs((*buffer)[i]);
        }
    }

    for (u64 i = 0; i < numSamples; i++) {
        (*buffer)[i] /= irMax;
    }

    fclose(wavFile);

    return numSamples;
}


void IRLoader::deallocateFFTEngine() {
    if (fftEngine) {
        pffft_destroy_setup(fftEngine);
    }
    
    pffft_aligned_free(inputBufferPaddedL);
    pffft_aligned_free(inputBufferPaddedR);
    pffft_aligned_free(inputDftBufferL);
    pffft_aligned_free(inputDftBufferR);
    pffft_aligned_free(irDftBuffer);
    pffft_aligned_free(convolutionResultBufferL);
    pffft_aligned_free(convolutionResultBufferR);
    pffft_aligned_free(convolutionResultDftBufferL);
    pffft_aligned_free(convolutionResultDftBufferR);
    pffft_aligned_free(fftWorkBuffer);
    free(overlapAddBufferL);
    free(overlapAddBufferR);
}

void IRLoader::reallocFFTEngine(u64 newSize) {
    
    if (newSize == fftSize) { return; }

    fftSize = newSize;
        
    deallocateFFTEngine();
    
    
    fftEngine = pffft_new_setup((int)fftSize, PFFFT_REAL);

    inputBufferPaddedL          = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    inputBufferPaddedR          = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    inputDftBufferL             = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    inputDftBufferR             = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    irDftBuffer                 = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));    
    convolutionResultBufferL    = (float *)pffft_aligned_malloc(2 * fftSize * sizeof(float));
    convolutionResultBufferR    = (float *)pffft_aligned_malloc(2 * fftSize * sizeof(float));
    convolutionResultDftBufferL = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    convolutionResultDftBufferR = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    overlapAddBufferL           = (float *)calloc(2 * fftSize, sizeof(float));
    overlapAddBufferR           = (float *)calloc(2 * fftSize, sizeof(float));
    
    if (fftSize >= 16384) {
        fftWorkBuffer = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    } else {
        fftWorkBuffer = nullptr;
    }

    for (u32 i = 0; i < fftSize; i++) {
        inputBufferPaddedL[i] = 0.0f;
        inputBufferPaddedR[i] = 0.0f;
    }
}


IRLoader::~IRLoader() {
    deallocateFFTEngine();
}


void IRLoader::init(double _samplerate, size_t _blockSize) {
    samplerate = _samplerate;
    overlapAddIndex = 0;
    blockSize = _blockSize;

    loadIR(true);
}

void IRLoader::prepareConvolution(float *irPtr, size_t irSize) {
    
    u64 newfftSize = nextPowTwo(irSize + 100);

    reallocFFTEngine(newfftSize);
    
    float *irTimeVec = (float *)pffft_aligned_malloc(fftSize * sizeof(float));

    for (u32 i = 0; i < irSize; i++) {
        irTimeVec[i] = irPtr[i];
    }

    for (u32 i = 0; i < (fftSize - irSize); i++) {
        irTimeVec[irSize + i] = 0.0f;
    }

    pffft_transform(fftEngine, irTimeVec, irDftBuffer, fftWorkBuffer, PFFFT_FORWARD);

    convolutionResultSize = irSize + blockSize - 1;
    updateIR = false;

    pffft_aligned_free(irTimeVec);

    return;
}

IRLoaderError IRLoader::loadIR(bool initIR) {
    
    defaultIR = initIR;

    if (initIR) {
        prepareConvolution(baseIR, BASE_IR_SIZE);
        return IRLoaderError::OK;
    }

    std::string irPath = irFile.getFullPathName().toStdString();

    if (irPath == "") { return IRLoaderError::Error; }

    size_t irSize = parseWavFile(irPath, &irBuffer);

    if (irSize == 0) { return IRLoaderError::Error; }

    irBufferSize = irSize;
    
    // the IR is fully loaded at the end of the process function
    updateIR = true;
    
    return IRLoaderError::OK;
}


void IRLoader::process(float *bufferL, float *bufferR, size_t nSamples) {

    if (bypass) { return; }
    
    for (size_t i = 0; i < nSamples; i++) {
        inputBufferPaddedL[i] = bufferL[i];
    }

    pffft_transform(fftEngine, inputBufferPaddedL, inputDftBufferL, fftWorkBuffer, PFFFT_FORWARD);

    for (size_t i = 0; i < fftSize; i++) {
        convolutionResultDftBufferL[i] = 0.0f;
    }

    pffft_zconvolve_accumulate(fftEngine, inputDftBufferL, irDftBuffer, convolutionResultDftBufferL, 1.0f);

    pffft_transform(fftEngine, convolutionResultDftBufferL, convolutionResultBufferL, fftWorkBuffer, PFFFT_BACKWARD);

    size_t overlapAddBufferSize = 2 * fftSize;

    // mettre les samples dans l'overlap add
    for (size_t i = 0; i < convolutionResultSize; i++) {
        size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
        overlapAddBufferL[index] += convolutionResultBufferL[i];
    }

    // mettre les samples dans le buffer de sortie et effacer les samples qui sont déjà sortis
    Sample outputScaling = 1.0f / fftSize;
    for (size_t i = 0; i < nSamples; i++) {
        size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
        bufferL[i] = overlapAddBufferL[index] * outputScaling;
        overlapAddBufferL[index] = 0.0f;
    }


    if (bufferR) {
    
        for (size_t i = 0; i < nSamples; i++) {
            inputBufferPaddedR[i] = bufferR[i];
        }
    
        pffft_transform(fftEngine, inputBufferPaddedR, inputDftBufferR, fftWorkBuffer, PFFFT_FORWARD);
    
        for (size_t i = 0; i < fftSize; i++) {
            convolutionResultDftBufferR[i] = 0.0f;
        }
    
        pffft_zconvolve_accumulate(fftEngine, inputDftBufferR, irDftBuffer, convolutionResultDftBufferR, 1.0f);
    
        pffft_transform(fftEngine, convolutionResultDftBufferR, convolutionResultBufferR, fftWorkBuffer, PFFFT_BACKWARD);
    
        overlapAddBufferSize = 2 * fftSize;
    
        // mettre les samples dans l'overlap add
        for (size_t i = 0; i < convolutionResultSize; i++) {
            size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
            overlapAddBufferR[index] += convolutionResultBufferR[i];
        }
    
        // mettre les samples dans le buffer de sortie et effacer les samples qui sont déjà sortis
        outputScaling = 1.0f / fftSize;
        for (size_t i = 0; i < nSamples; i++) {
            size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
            bufferR[i] = overlapAddBufferR[index] * outputScaling;
            overlapAddBufferR[index] = 0.0f;
        }
    }
 
    overlapAddIndex = (overlapAddIndex + (int)nSamples) % overlapAddBufferSize;
    
    // update IR here to make sure nothing changes during processing 
    if (updateIR) { 
        prepareConvolution(irBuffer, irBufferSize); 
    }

    return;
}
