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

struct ChunkData {

    char riffString[4];
    int32_t fileSize; 
    char format[4];

    char subChunk1ID[4];
    int32_t subChunk1Size;
    int16_t audioFormat;
    int16_t numChannels;
    int32_t samplerate;
    int32_t byteRate;
    int16_t blockAlign;
    int16_t bitsPerSample;
    
    char subChunk2ID[4];
    int32_t signalSizeBytes;
};

struct WavSignal {
    float *data = nullptr;
    u64 size = 0;
    
    ~WavSignal() {
        if (data) { free(data); }
    }
};

static u64 parseWavFile(const std::string& filepath, float **outputBuffer) {

    FILE *wavFile = nullptr;
    fopen_s(&wavFile, filepath.data(), "rb");
    
    if (ferror(wavFile)) {
        fclose(wavFile);
        return 0;
    }
    
    ChunkData chunk_data;
    
    fread(chunk_data.riffString, 1, 4, wavFile);

    if (memcmp(chunk_data.riffString, "RIFF", 4) != 0) {
        juce::AlertWindow::showMessageBox(
            juce::MessageBoxIconType::WarningIcon, 
            "IR file error",
            "There is an error in the \"RIFF\" header of the .wav file, please choose another file", 
            "Ok");
        
        fclose(wavFile);
        return 0;
    }

    fread((int8_t *)(&chunk_data.fileSize), 1, 4, wavFile);
    
    int8_t *file_data = (int8_t *)malloc(chunk_data.fileSize);
    fread(file_data, 1, chunk_data.fileSize, wavFile);
    int file_data_index = 0;
        
    memcpy(chunk_data.format, file_data + file_data_index, 4);
    file_data_index += 4;
    
    assert(memcmp(chunk_data.format, "WAVE", 4) == 0);
    if (memcmp(chunk_data.format, "WAVE", 4) != 0) {
        juce::AlertWindow::showMessageBox(
            juce::MessageBoxIconType::WarningIcon, 
            "IR file error",
            "There is an error in the \"WAVE\" header of the .wav file, please choose another file", 
            "Ok");
        
        fclose(wavFile);
        return 0;
    }
    
    printf("RIFF String = %4s \n", chunk_data.riffString);
    printf("fileSize = %d \n", chunk_data.fileSize);
    printf("format = %4s \n", chunk_data.format);
    
    while (memcmp(file_data + file_data_index, "fmt ", 4)) {
        file_data_index++;
    }
    
    memcpy(chunk_data.subChunk1ID, file_data + file_data_index, 4);
    file_data_index += 4;


    chunk_data.subChunk1Size = *(int32_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.subChunk1Size);    
    assert(memcmp(chunk_data.subChunk1ID, "fmt ", 4) == 0);
    
    chunk_data.audioFormat = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.audioFormat);
    
    chunk_data.numChannels = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.numChannels);
    
    chunk_data.samplerate = *(int32_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.samplerate);
    
    chunk_data.byteRate = *(int32_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.byteRate);
    
    chunk_data.blockAlign = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.blockAlign);
    
    chunk_data.bitsPerSample = *(int16_t *)(file_data + file_data_index); 
    file_data_index += sizeof(chunk_data.bitsPerSample);
        
    assert(chunk_data.subChunk1Size == 16);
    assert(chunk_data.audioFormat == 1);
    assert(chunk_data.numChannels == 1); // only mono signals
    assert(chunk_data.byteRate == chunk_data.samplerate * chunk_data.numChannels * chunk_data.bitsPerSample / 8);
    assert(chunk_data.blockAlign == chunk_data.numChannels * chunk_data.bitsPerSample / 8);    
        
    if (chunk_data.numChannels != 1) {
        juce::AlertWindow::showMessageBox(
            juce::MessageBoxIconType::WarningIcon, 
            "IR file error",
            "Sorry, I don't support IRs with more than one channels for now, please choose another file", 
            "Ok");
        
        fclose(wavFile);
        return 0;        
    }
    
    printf("subChunk1ID = %4s \n", chunk_data.subChunk1ID);
    printf("subChunk1Size = %d \n", chunk_data.subChunk1Size);
    printf("audioFormat = %d \n", chunk_data.audioFormat);

    printf("numChannels = %d \n", chunk_data.numChannels);
    printf("samplerate = %d \n", chunk_data.samplerate);
    printf("byteRate = %d \n", chunk_data.byteRate);
    printf("blockAlign = %d \n", chunk_data.blockAlign);
    printf("bitsPerSample = %d \n", chunk_data.bitsPerSample);

    
    while (memcmp(file_data + file_data_index, "data", 4)) {
        file_data_index++;
    }
    memcpy(chunk_data.subChunk2ID, file_data + file_data_index, 4);
    file_data_index += 4;
    
    assert(memcmp(chunk_data.subChunk2ID, "data", 4) == 0);

    if (memcmp(chunk_data.subChunk2ID, "data", 4) != 0) {
        juce::AlertWindow::showMessageBox(
            juce::MessageBoxIconType::WarningIcon, 
            "IR file error",
            "The file contains anoying meta data than I can't decode for now, please choose another file", 
            "Ok");
        
        fclose(wavFile);
        return 0;        
    }
    
    printf("subChunk2ID = %4s \n", chunk_data.subChunk2ID);

    chunk_data.signalSizeBytes = *(int32_t*)(file_data + file_data_index);
    file_data_index += sizeof(chunk_data.signalSizeBytes);
        
    printf("signalSizeBytes = %d \n", chunk_data.signalSizeBytes);
    
    size_t sampleSizeBytes = chunk_data.blockAlign/chunk_data.numChannels;
    size_t numSamples = chunk_data.signalSizeBytes/sampleSizeBytes;

    *outputBuffer = (float *)calloc(numSamples, sizeof(float));
    
    int8_t *signal_ptr = file_data + file_data_index;
    
    uint32_t byte_counter = 0;
    uint32_t sampleCounter = 0;
    
    if (chunk_data.bitsPerSample == 16) {

        int8_t sampleChar[2] = {0x00, 0x00};
        
        while (sampleCounter < numSamples && byte_counter < (uint32_t)chunk_data.signalSizeBytes) {
            
            memcpy(sampleChar, signal_ptr + byte_counter, 2);
            byte_counter += 2;
                        
            int32_t mask = 0x01 << (chunk_data.bitsPerSample - 1);
            int16_t sampleInt = *(int16_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFF;
            }            

            float sample = sampleInt / (float)_I16_MAX * 0.5f;

            (*outputBuffer)[sampleCounter] = sample;
            sampleCounter++;
        }

    } else if (chunk_data.bitsPerSample == 24) {
    
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};
        while (sampleCounter < numSamples && byte_counter < (uint32_t)chunk_data.signalSizeBytes) {
            
            memcpy(sampleChar, signal_ptr + byte_counter, 3);
            byte_counter += 3;

            int64_t mask = 0x01i64 << (chunk_data.bitsPerSample - 1);
            // uint32_t mask = 0x800000;
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFF;
            }

            float sample = (float)(sampleInt / BITS_24_MAX * 0.5f);

            (*outputBuffer)[sampleCounter] = sample;
            sampleCounter++;
        }

    } else if (chunk_data.bitsPerSample == 32) {
        
        int8_t sampleChar[4] = {0x00, 0x00, 0x00, 0x00};

        while (sampleCounter < numSamples && byte_counter < (uint32_t)chunk_data.signalSizeBytes) {

            memcpy(sampleChar, signal_ptr + byte_counter, 4);
            byte_counter += 4;
            
            int64_t mask = 0x01i64 << (chunk_data.bitsPerSample - 1);
            int32_t sampleInt = *(int32_t *)sampleChar;

            if (sampleInt & mask) {
                sampleInt |= ~0xFFFFFFFF;
            }

            float sample = sampleInt / (float)_I32_MAX * 0.5f;

            (*outputBuffer)[sampleCounter] = sample;
            sampleCounter++;
        }
    }


    float irMax = 0.0f;

    for (u64 i = 0; i < (u64)(numSamples/4); i++) {
        if (abs((*outputBuffer)[i]) > abs(irMax)) {
            irMax = abs((*outputBuffer)[i]);
        }
    }

    for (u64 i = 0; i < numSamples; i++) {
        (*outputBuffer)[i] /= irMax;
    }

    free(file_data);
    fclose(wavFile);

    return numSamples;
}


void IRLoader::deallocateFFTEngine() {
    if (fftEngine) {
        pffft_destroy_setup(fftEngine);
    }
    
    pffft_aligned_free(inputBufferPadded);
    pffft_aligned_free(inputDftBuffer);
    pffft_aligned_free(irDftBuffer);
    pffft_aligned_free(convolutionResultBuffer);
    pffft_aligned_free(convolutionResultDftBuffer);
    pffft_aligned_free(fftWorkBuffer);
    free(overlapAddBufferL);
    free(overlapAddBufferR);
}

void IRLoader::reallocFFTEngine(u64 newSize) {
    
    if (newSize == fftSize) { return; }

    fftSize = newSize;
        
    deallocateFFTEngine();
    
    
    fftEngine = pffft_new_setup((int)fftSize, PFFFT_REAL);

    inputBufferPadded           = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    inputDftBuffer              = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    irDftBuffer                 = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));    
    convolutionResultBuffer     = (float *)pffft_aligned_malloc(2 * fftSize * sizeof(float));
    convolutionResultDftBuffer  = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    overlapAddBufferL           = (float *)calloc(2 * fftSize, sizeof(float));
    overlapAddBufferR           = (float *)calloc(2 * fftSize, sizeof(float));
    
    if (fftSize >= 16384) {
        fftWorkBuffer = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    } else {
        fftWorkBuffer = nullptr;
    }

    for (u32 i = 0; i < fftSize; i++) {
        inputBufferPadded[i] = 0.0f;
        inputBufferPadded[i] = 0.0f;
    }
}


IRLoader::~IRLoader() {
    deallocateFFTEngine();
    
    if (irBuffer) { free(irBuffer); }
}


void IRLoader::init(double _samplerate, size_t _blockSize) {
    samplerate = _samplerate;
    overlapAddIndex = 0;
    blockSize = _blockSize;

    loadIR();
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

IRLoaderError IRLoader::loadIR() {
    
    if (defaultIR) {
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
    
    for (size_t i = 0; i < nSamples; i++) {
        inputBufferPadded[i] = bufferL[i];
    }

    pffft_transform(fftEngine, inputBufferPadded, inputDftBuffer, fftWorkBuffer, PFFFT_FORWARD);

    for (size_t i = 0; i < fftSize; i++) {
        convolutionResultDftBuffer[i] = 0.0f;
    }

    pffft_zconvolve_accumulate(fftEngine, inputDftBuffer, irDftBuffer, convolutionResultDftBuffer, 1.0f);

    pffft_transform(fftEngine, convolutionResultDftBuffer, convolutionResultBuffer, fftWorkBuffer, PFFFT_BACKWARD);

    size_t overlapAddBufferSize = 2 * fftSize;

    // mettre les samples dans l'overlap add
    for (size_t i = 0; i < convolutionResultSize; i++) {
        size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
        overlapAddBufferL[index] += convolutionResultBuffer[i];
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
            inputBufferPadded[i] = bufferR[i];
        }
    
        pffft_transform(fftEngine, inputBufferPadded, inputDftBuffer, fftWorkBuffer, PFFFT_FORWARD);
    
        for (size_t i = 0; i < fftSize; i++) {
            convolutionResultDftBuffer[i] = 0.0f;
        }
    
        pffft_zconvolve_accumulate(fftEngine, inputDftBuffer, irDftBuffer, convolutionResultDftBuffer, 1.0f);
    
        pffft_transform(fftEngine, convolutionResultDftBuffer, convolutionResultBuffer, fftWorkBuffer, PFFFT_BACKWARD);
    
        overlapAddBufferSize = 2 * fftSize;
    
        // mettre les samples dans l'overlap add
        for (size_t i = 0; i < convolutionResultSize; i++) {
            size_t index = (overlapAddIndex + i) % overlapAddBufferSize;
            overlapAddBufferR[index] += convolutionResultBuffer[i];
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

