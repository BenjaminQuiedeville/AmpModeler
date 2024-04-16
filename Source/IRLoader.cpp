/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef JUCE_MODAL_LOOPS_PERMITTED
    #define JUCE_MODAL_LOOPS_PERMITTED 1
#endif

#include "IRLoader.h"
#include <memory>
#include <math.h>
#include <assert.h>
#include <fstream>
#include <iostream>


#define BITS_24_MAX (double)((1 << 23) - 1)

#define BASE_IR_SIZE 5793
static float baseIR[BASE_IR_SIZE] = {
#include "baseIR.inc"
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

    // assert(riffString[0] == 'R' || riffString[1] == 'I' 
    //     || riffString[2] == 'F' || riffString[3] == 'F');

    assert(memcmp(riffString, "RIFF", 4) == 0);
    if (memcmp(riffString, "RIFF", 4) != 0) {
        fclose(wavFile);
        return 0;
    }

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
    
    assert(NumChannels == 1);
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
    assert(memcmp(SubChunk2ID, "data", 4) == 0 && "le fichier contient des métadonnées chiantes");

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

            sample_t sample = (sample_t)sampleInt / (sample_t)BITS_24_MAX / 2.0f;

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


void IRLoader::reallocFFTEngine(u64 newSize) {
    
    if (newSize == fftSize) { return; }

    fftSize = newSize;
        
    pffft_destroy_setup(fftEngine);
    
    pffft_aligned_free(inputBufferPadded);
    pffft_aligned_free(convolutionResultBuffer);
    pffft_aligned_free(inputDftBuffer);
    pffft_aligned_free(irDftBuffer);
    pffft_aligned_free(fftWorkBuffer);
    free(overlapAddBuffer);
    
    
    fftEngine = pffft_new_setup((int)fftSize, PFFFT_REAL);

    inputBufferPadded       = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    convolutionResultBuffer = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    inputDftBuffer          = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    irDftBuffer             = (float *)pffft_aligned_malloc(fftSize * sizeof(float));    
    overlapAddBuffer        = (float *)malloc(2 * fftSize * sizeof(float));
    
    if (fftSize >= 16384) {
        fftWorkBuffer = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    } else {
        fftWorkBuffer = nullptr;
    }

    for (u32 i = 0; i < fftSize; i++) {
        inputBufferPadded[i] = 0.0f;
        overlapAddBuffer[i] = 0.0f;    
    }
    
}

IRLoader::IRLoader() {

    fftEngine = pffft_new_setup((int)fftSize, PFFFT_REAL);

    inputBufferPadded       = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    convolutionResultBuffer = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    inputDftBuffer          = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    irDftBuffer             = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    fftWorkBuffer           = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    overlapAddBuffer        = (float *)malloc(2 * fftSize * sizeof(float));

    for (u32 i = 0; i < fftSize; i++) {
        inputBufferPadded[i] = 0.0f;
        overlapAddBuffer[i] = 0.0f;    
    }    
}

IRLoader::~IRLoader() {

    pffft_destroy_setup(fftEngine);

    pffft_aligned_free(inputBufferPadded);       
    pffft_aligned_free(convolutionResultBuffer); 
    pffft_aligned_free(inputDftBuffer);          
    pffft_aligned_free(irDftBuffer);             
    pffft_aligned_free(fftWorkBuffer);
    free(overlapAddBuffer);        
}

void IRLoader::init(double _samplerate, size_t _blockSize) {
    samplerate = _samplerate;
    overlapAddIndex = 0;
    blockSize = _blockSize;

    loadIR(true);
}

void IRLoader::prepareConvolution(float *irPtr, size_t irSize) {
    
    u64 newfftSize = nextPowTwo(irSize);

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

void IRLoader::loadIR(bool initIR) {

    if (initIR) {
        prepareConvolution(baseIR, BASE_IR_SIZE);
        return;
    }

    std::string irPath = irFile.getFullPathName().toStdString();

    if (irPath == "") { return; }

    size_t irSize = parseWavFile(irPath, &irBuffer);

    if (irSize == 0) { return; }

    irBufferSize = irSize;
    
    // the IR is fully loaded at the end of the process function
    updateIR = true;
}


void IRLoader::process(float *input, size_t nSamples) {

    if (bypass) { return; }

    for (size_t i = 0; i < nSamples; i++) {
        inputBufferPadded[i] = input[i];
    }

    pffft_transform(fftEngine, inputBufferPadded, inputDftBuffer, fftWorkBuffer, PFFFT_FORWARD);

    pffft_zconvolve_accumulate(fftEngine, inputDftBuffer, irDftBuffer, inputDftBuffer, 1.0f);

    pffft_transform(fftEngine, inputDftBuffer, convolutionResultBuffer, fftWorkBuffer, PFFFT_BACKWARD);

    size_t overlapAddBufferSize = 2 * fftSize;

    // mettre les samples dans l'overlap add
    for (size_t i = 0; i < convolutionResultSize; i++) {
        size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
        overlapAddBuffer[index] += convolutionResultBuffer[i];
    }

    // mettre les samples dans le buffer de sortie et effacer les samples qui sont déjà sortis
    for (size_t i = 0; i < nSamples; i++) {
        size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
        input[i] = overlapAddBuffer[index] / fftSize;
        overlapAddBuffer[index] = 0.0f;
    }

    overlapAddIndex = (overlapAddIndex + (int)nSamples) % overlapAddBufferSize;

    
    // update IR here to make sure nothing changes during processing 
    if (updateIR) { 
        prepareConvolution(irBuffer, irBufferSize); 
    }

    return;
}
