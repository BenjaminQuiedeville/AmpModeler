/*
  ==============================================================================

    IRLoader.cpp
    Created: 22 Apr 2023 5:34:02pm
    Author:  Benjamin

  ==============================================================================
*/

#include "IRLoader.h"
#include <memory>
#include <math.h>
#include <assert.h>
#include <fstream>
#include <iostream>
#include <vector>

#define BITS_24_MAX (double)(1 << 23 - 1)

#define BASE_IR_SIZE 5793
static float baseIR[BASE_IR_SIZE] = {
#include "baseIR.inc"
};

static size_t parseWavFile(const std::string& filepath, float **buffer) {


    char temp[1] = {0};
    char riffString[4];
    int32_t ChunkSize; 
    char format[4];

    char SubChunk1ID[4];
    int32_t SubChunk1Size;
    int16_t AudioFormat;
    int16_t NumChannels;
    int32_t samplerate;
    int32_t byteRate;
    int16_t BlockAlign;
    int16_t bitsPerSample;
    
    char SubChunk2ID[4];
    int32_t signalSizeBytes;

    FILE *wavFile = fopen(filepath.data(), "rb");

    assert(!ferror(wavFile));

    fread(riffString, 1, 4, wavFile);

    assert(riffString[0] == 'R' || riffString[1] == 'I' 
        || riffString[2] == 'F' || riffString[3] == 'F');

    // if (riffString[0] != 'R' || riffString[1] != 'I' 
    //  || riffString[2] != 'F' || riffString[3] != 'F')
    // {
    //     return 0;
    // }

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
    
    // if (NumChannels != 1) { 
    //     return 0; 
    // }

    // if (byteRate != (samplerate * NumChannels * bitsPerSample/8)) {
    //     printf("byte Rate errone");
    // }

    // if (BlockAlign != (NumChannels * bitsPerSample/8)) {
    //     printf("BlockAlign Rate errone");
    // }

    while (temp[0] != 'd') {
        fread(temp, 1, 1, wavFile);
    }

    SubChunk2ID[0] = temp[0];
    temp[0] = 0;
    fread(SubChunk2ID+1, 1, 3, wavFile);
    

    if (SubChunk2ID[0] != 'd' || SubChunk2ID[1] != 'a'
     || SubChunk2ID[2] != 't' || SubChunk2ID[3] != 'a')
    {
        while (temp[0] != 'd') {
            fread(temp, 1, 1, wavFile);
        }

        SubChunk2ID[0] = temp[0];
        fread(SubChunk2ID+1, 1, 3, wavFile);

    }

    assert((SubChunk2ID[0] == 'd' || SubChunk2ID[1] == 'a' 
         || SubChunk2ID[2] == 't' || SubChunk2ID[3] == 'a'));

    // printf("SubChunk2ID = %4s \n", SubChunk2ID);
    // printf("signalSizeBytes = %d \n", signalSizeBytes);

    fread(&signalSizeBytes, 4, 1, wavFile);
    
    size_t sampleSizeBytes = BlockAlign/NumChannels;
    size_t numSamples = signalSizeBytes/sampleSizeBytes;


    // std::vector<float> *buffervec = new std::vector<float>()
    *buffer = (float *)calloc(numSamples, sizeof(float));
    assert(buffer != nullptr);

    uint32_t counter = 0;
    if (bitsPerSample == 16) {

        int8_t sampleChar[2] = {0x00, 0x00};

        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 2, wavFile);
            
            int32_t mask = (int32_t)1 << (bitsPerSample - 1);
            int16_t sampleInt = *(int16_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFF;
            }            

            float sample = sampleInt / (float)_I16_MAX;

            (*buffer)[counter] = sample;
            counter++;
        }

    } else if (bitsPerSample == 24) {
    
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};
        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 3, wavFile);

            int64_t mask = (int64_t)1 << (bitsPerSample - 1);
            // uint32_t mask = 0x800000;
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFF;
            }

            float sample = sampleInt / BITS_24_MAX / 2.0;

            (*buffer)[counter] = sample;
            counter++;
        }

    } else if (bitsPerSample == 32) {
        
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};

        while (!feof(wavFile) && counter < numSamples) {

            fread(sampleChar, 1, 4, wavFile);

            int64_t mask = (int64_t)1 << (bitsPerSample - 1);
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFFFF;
            }

            float sample = sampleInt / (float)_I32_MAX;

            (*buffer)[counter] = sample;
            counter++;
        }
    }

    fclose(wavFile);

    return numSamples;
}


IRLoader::IRLoader() {

    fftEngine = new FFT(false);

    inputBufferPadded = fftEngine->createTimeVector();
    inputDftBuffer = fftEngine->createFreqVector();
    irDftBuffer = fftEngine->createFreqVector();
    convolutionResultBuffer = fftEngine->createTimeVector();
    overlapAddBuffer = fftEngine->createTimeVector();

}

IRLoader::~IRLoader() {

    delete fftEngine;
}

void IRLoader::init(double _samplerate, int _blockSize) {

    initIR = true;
    overlapAddIndex = 0;
    blockSize = _blockSize;

    loadIR();
}

void IRLoader::prepareConvolution(const float *irPtr, int irSize) {


    FFT::TimeVector irTimeVec = fftEngine->createTimeVector();
    
    assert(irSize < FFT_SIZE);

    for (size_t i = 0; i < irSize; i++) {
        irTimeVec[i] = irPtr[i];
    }

    fftEngine->forward(irTimeVec, irDftBuffer);
    fftEngine->scale(irDftBuffer);

    convolutionResultSize = irSize + blockSize - 1;
    return;

}


void IRLoader::loadIR() {

    if (initIR) {
        prepareConvolution(baseIR, BASE_IR_SIZE);
        initIR = false;
        return;
    }

    float *irBuffer = nullptr;

    juce::FileChooser *chooser = new juce::FileChooser("Choose a .wav File to open", juce::File(), "*.wav");

    chooser->browseForFileToOpen();
    const std::string filepath = chooser->getResult().getFullPathName().toStdString();

    size_t irSize = parseWavFile(filepath, &irBuffer);

    if (irSize != 0) {
        prepareConvolution(irBuffer, irSize);
    }

    if (irBuffer != nullptr) { free(irBuffer); }
    
    delete chooser;
    return;
}


void IRLoader::process(float *input, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {
        inputBufferPadded[i] = input[i];
    }

    fftEngine->forward(inputBufferPadded, inputDftBuffer);
    fftEngine->scale(inputDftBuffer);

    for (size_t i = 0; i < fftEngine->spectrum_size; i++) {
        inputDftBuffer[i] *= irDftBuffer[i];
    }
    
    fftEngine->inverse(inputDftBuffer, convolutionResultBuffer);

    // pffft_transform(fftSetup, inputBufferPadded, inputDftBuffer, nullptr, PFFFT_FORWARD);
    // pffft_zconvolve_accumulate(fftSetup, inputDftBuffer, irDftBuffer, convolutionResultBuffer, 1/fftSize);    
    // pffft_transform(fftSetup, inputDftBuffer, inputBufferPadded, nullptr, PFFFT_BACKWARD);

    // clear les samples précédents pour éviter le recouvrement avec des samples passés
    for (size_t i = 0; i < nSamples; i++) {
        int index = overlapAddIndex - i - 1;
        if (index < 0) { index += nSamples; }
        index %= nSamples;
        overlapAddBuffer[i] = 0.0f;
    }


    // mettre les samples dans l'overlap add
    for (size_t i = 0; i < convolutionResultSize; i++) {
        int index = (overlapAddIndex + i) % convolutionResultSize;
        overlapAddBuffer[index] += convolutionResultBuffer[i];
    }

    // mettre les samples dans le buffer de sortie
    for (int i = 0; i < nSamples; i++) {
        int index = (overlapAddIndex + i) % convolutionResultSize;
        input[i] = overlapAddBuffer[index];
    }

    for (int i = 0; i < nSamples; i++) {
        input[i] = inputBufferPadded[i];
    }

    overlapAddIndex = (overlapAddIndex + nSamples) % (FFT_SIZE);

    return;
}
