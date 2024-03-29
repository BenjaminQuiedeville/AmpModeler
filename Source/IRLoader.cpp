/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "IRLoader.h"
#include <memory>
#include <math.h>
#include <assert.h>
#include <fstream>
#include <iostream>

#ifndef JUCE_MODAL_LOOPS_PERMITTED
    #define JUCE_MODAL_LOOPS_PERMITTED 1
#endif

#define BITS_24_MAX (double)((1 << 23) - 1)

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
    
    size_t sampleSizeBytes = BlockAlign/NumChannels;
    size_t numSamples = signalSizeBytes/sampleSizeBytes;

    // allocate the IR buffer
    if (*buffer != nullptr) {
        *buffer = (float *)realloc(*buffer, numSamples * sizeof(float));
        memset(*buffer, 0, numSamples * sizeof(float));

    } else {
        *buffer = (float *)calloc(numSamples, sizeof(float));
    }


    uint32_t counter = 0;
    switch (bitsPerSample) {
    case 16: {

        int8_t sampleChar[2] = {0x00, 0x00};

        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 2, wavFile);
            
            int32_t mask = (int32_t)(1 << (bitsPerSample - 1));
            int16_t sampleInt = *(int16_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFF;
            }            

            float sample = sampleInt / (float)_I16_MAX;

            (*buffer)[counter] = sample;
            counter++;
        }
    }
    case 24: {
    
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};
        while (!feof(wavFile) && counter < numSamples) {
            fread(sampleChar, 1, 3, wavFile);

            int64_t mask = (int64_t)1 << (bitsPerSample - 1);
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFF;
            }

            sample_t sample = (sample_t)sampleInt / (sample_t)BITS_24_MAX / 2.0f;

            (*buffer)[counter] = sample;
            counter++;
        }
    }
    case 32: {
        
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
    }

    float irMax = 0.0f;

    for (size_t i = 0; i < (size_t)(numSamples/4); i++) {
        if (abs((*buffer)[i]) > abs(irMax)) {
            irMax = abs((*buffer)[i]);
        }
    }

    for (size_t i = 0; i < numSamples; i++) {
        (*buffer)[i] /= irMax;
    }

    fclose(wavFile);

    return numSamples;
}


IRLoader::IRLoader() {

    fftEngine = new FFT(false);

    inputBufferPadded          = fftEngine->createTimeVector();
    inputDftBuffer             = fftEngine->createFreqVector();
    irDftBuffer                = fftEngine->createFreqVector();
    convolutionResultBuffer    = fftEngine->createTimeVector();
    overlapAddBuffer           = fftEngine->createTimeVector();

}

IRLoader::~IRLoader() {

    delete fftEngine;

}

void IRLoader::init(double _samplerate, size_t _blockSize) {
    samplerate = _samplerate;
    overlapAddIndex = 0;
    blockSize = _blockSize;

    loadIR(true, nullptr);
}

//@TODO not thread safe
void IRLoader::prepareConvolution(float *irPtr, size_t irSize) {

    FFT::TimeVector irTimeVec = fftEngine->createTimeVector();
    
    assert(irSize < FFT_SIZE);

    for (size_t i = 0; i < irSize; i++) {
        irTimeVec[i] = irPtr[i];
    }

    fftEngine->forward(irTimeVec, irDftBuffer);
    fftEngine->scale(irDftBuffer);

    convolutionResultSize = irSize + blockSize - 1;
    updateIR = false;

    return;
}


void IRLoader::loadIR(bool initIR, juce::Label *irNameLabel) {

    if (initIR) {
        prepareConvolution(baseIR, BASE_IR_SIZE);
        return;
    }


    auto chooser = std::make_unique<juce::FileChooser>("Choose a .wav File to open", juce::File(), "*.wav");

    bool fileChoosed = chooser->browseForFileToOpen();
    if (!fileChoosed) { 
        return; 
    }

    juce::File returnedFile = chooser->getResult();
    const std::string filepath = returnedFile.getFullPathName().toStdString();

    if (filepath == "") { return; }

    size_t irSize = parseWavFile(filepath, &irBuffer);

    if (irSize == 0) { return; }

    // prepareConvolution(irBuffer, irSize);
    irBufferSize = irSize;
    updateIR = true;

    // if (irBuffer != nullptr) { free(irBuffer); }
    
    assert(irNameLabel != nullptr);
    irNameLabel->setText(returnedFile.getFileNameWithoutExtension(),
                         juce::NotificationType::dontSendNotification);

    return;
}


void IRLoader::process(float *input, size_t nSamples) {

    if (bypass) { return; }

    for (size_t i = 0; i < nSamples; i++) {
        inputBufferPadded[i] = input[i];
    }

    // memcpy(inputBufferPadded.data(), input, nSamples*sizeof(sample_t));

    fftEngine->forward(inputBufferPadded, inputDftBuffer);
    // fftEngine->scale(inputDftBuffer);

    for (size_t i = 0; i < fftEngine->spectrum_size; i++) {
        inputDftBuffer[i] *= irDftBuffer[i];
    }

    
    fftEngine->inverse(inputDftBuffer, convolutionResultBuffer);

    // pffft_transform(fftSetup, inputBufferPadded, inputDftBuffer, nullptr, PFFFT_FORWARD);
    // pffft_zconvolve_accumulate(fftSetup, inputDftBuffer, irDftBuffer, convolutionResultBuffer, 1/fftSize);    
    // pffft_transform(fftSetup, inputDftBuffer, inputBufferPadded, nullptr, PFFFT_BACKWARD);

    size_t overlapAddBufferSize = overlapAddBuffer.size();

    // mettre les samples dans l'overlap add
    for (size_t i = 0; i < convolutionResultSize; i++) {
        size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
        overlapAddBuffer[index] += convolutionResultBuffer[i];
    }

    // mettre les samples dans le buffer de sortie
    for (size_t i = 0; i < nSamples; i++) {
        size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
        input[i] = overlapAddBuffer[index];
        overlapAddBuffer[index] = 0.0f;
    }

    overlapAddIndex = (overlapAddIndex + (int)nSamples) % overlapAddBufferSize;

    if (updateIR) { 
        prepareConvolution(irBuffer, irBufferSize); 
    }

    return;
}
