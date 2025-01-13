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

#define DR_WAV_IMPLEMENTATION
#include <dr_libs/dr_wav.h>

#define BASE_IR_441_SIZE 1323
static float baseIR_441[BASE_IR_441_SIZE] = {
#include "data/baseIR_441.txt"
};

#define BASE_IR_48_SIZE 1440
static float baseIR_48[BASE_IR_48_SIZE] = {
#include "data/baseIR_48.txt"
};


void IRLoader::deallocateFFTEngine() {
    if (fftSetup) {
        pffft_destroy_setup(fftSetup);
    }
    
    if (irDftBuffers) {
        pffft_aligned_free(irDftBuffers[0]);
    }
    
    if (FDL) {
        pffft_aligned_free(FDLMemory);
    }
    
    free(irDftBuffers);
    free(FDL);

    pffft_aligned_free(convolutionDftResult);
    pffft_aligned_free(fftTimeInputBuffer);
    pffft_aligned_free(fftTimeOutputBuffer);
}

IRLoader::~IRLoader() {
    deallocateFFTEngine();
    
    if (irBuffer) { free(irBuffer); }
}


void IRLoader::init(double _samplerate, size_t _blockSize) {
    samplerate = _samplerate;
    blockSize = _blockSize;

    loadIR();
}

void IRLoader::prepareConvolution(float *irPtr, size_t irSize) {
    
    numIRParts = (u32)(irSize / blockSize);
    if (irSize % blockSize != 0) { numIRParts += 1; } 

    assert(nextPowTwo(blockSize) == blockSize && "IRLoader, blockSize must be a power of 2!");
    
    fftSize = 2*blockSize;
    dftSize = fftSize + 2;
    deallocateFFTEngine();
    
    fftSetup = pffft_new_setup((int)fftSize, PFFFT_REAL);
    
    u64 part_size_bytes = dftSize * sizeof(float);
    u64 padding = part_size_bytes & 0xF; 


    irDftBuffers = (float**)calloc(numIRParts, sizeof(float*));
    irDftBuffers[0] = (float*)pffft_aligned_malloc(numIRParts * (part_size_bytes+padding));
    
    FDL = (float**)calloc(numIRParts, sizeof(float*));
    FDLMemory = (float*)pffft_aligned_malloc(numIRParts * (part_size_bytes+padding));
    memset(FDLMemory, 0, numIRParts * (part_size_bytes+padding));
    FDL[0] = FDLMemory;
    
    convolutionDftResult = (float*)pffft_aligned_malloc(part_size_bytes);

    fftTimeInputBuffer = (float*)pffft_aligned_malloc(fftSize * sizeof(float));
    fftTimeOutputBuffer = (float*)pffft_aligned_malloc(fftSize * sizeof(float));
    
    for (u32 index = 1; index < numIRParts; index++) {
        irDftBuffers[index] = (float*)((uintptr_t)irDftBuffers[index-1] + (part_size_bytes+padding));
        FDL[index]          = (float*)((uintptr_t)FDL[index-1] + (part_size_bytes+padding));
    }

    u32 baseIRIndex = 0;
    u32 partSize = 0;
    for (u32 index = 0; index < numIRParts; index++) {
        baseIRIndex = (u32)(index * blockSize);
        
        if (baseIRIndex + blockSize >= irSize) {
            partSize = (u32)(irSize - baseIRIndex);    
        } else {
            partSize = (u32)blockSize;
        }
        
        memset(fftTimeInputBuffer, 0, fftSize * sizeof(float));
        memcpy(fftTimeInputBuffer, &irPtr[baseIRIndex], partSize * sizeof(float));
        
        pffft_transform(fftSetup, fftTimeInputBuffer, irDftBuffers[index], nullptr, PFFFT_FORWARD);
    }

    memset(fftTimeInputBuffer, 0, fftSize * sizeof(float));
    memset(fftTimeOutputBuffer, 0, fftSize * sizeof(float));
    
    updateIR = false;
}

IRLoaderError IRLoader::loadIR() {
    
    if (defaultIR) {        
        if (samplerate == 44100.0) {
            prepareConvolution(baseIR_441, BASE_IR_441_SIZE);
        
        } else if (samplerate == 48000.0) {
            prepareConvolution(baseIR_48, BASE_IR_48_SIZE);
        } else {
            juce::AlertWindow::showMessageBox(
                juce::MessageBoxIconType::WarningIcon, 
                "Samplerate error",
                "Sorry, the default provided IR is only available at 44.1 or 48kHz, if you work at another sampling rate, please provide your own IR.", 
                "Ok");        
            return IRLoaderError::Error;
        }
        
        return IRLoaderError::OK;
    }

    std::string irPath = irFile.getFullPathName().toStdString();
    if (irPath == "") { return IRLoaderError::Error; }
    
    IRLoaderError error = IRLoaderError::OK;

    {
        drwav wav;
        drwav_uint64 numFramesRead = 0;
        bool result = drwav_init_file(&wav, irPath.data(), NULL);
        
        if (!result) {
            juce::AlertWindow::showMessageBox(
                juce::MessageBoxIconType::WarningIcon, 
                "IR file error",
                "Error during the opening of the wav file, try another one sorry", 
                "Ok");
            error = IRLoaderError::Error;
            goto wav_opening_scope_end;
        }
    
        if (wav.channels > 1) {
            juce::AlertWindow::showMessageBox(
                juce::MessageBoxIconType::WarningIcon, 
                "IR file error",
                "Sorry, I don't support IRs with more than one channels for now, please choose another file", 
                "Ok");
            error = IRLoaderError::Error;
            goto wav_opening_scope_end;
        }
        
        irBuffer = (float*)calloc(wav.totalPCMFrameCount, sizeof(float));
        numFramesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, irBuffer);
        
        assert(numFramesRead == wav.totalPCMFrameCount && "Error in decoding the wav file");
        irBufferSize = (u32)numFramesRead;

        // the IR is fully loaded at the end of the process function
        updateIR = true;
    
        wav_opening_scope_end: {
            drwav_uninit(&wav);
        }
    }

    return error;
}


void IRLoader::process(float *bufferL, float *bufferR, size_t nSamples) {
    
    (void*)bufferR;
    u32 index = 0;
    
    for (index = 0; index < fftSize-nSamples; index++) {
        fftTimeInputBuffer[index] = fftTimeInputBuffer[index + nSamples];
    }
    
    for (index = 0; index < nSamples; index++) {
        fftTimeInputBuffer[fftSize-nSamples + index] = bufferL[index];
    }
        
    
    //shift the FDL
    float* tempPtr = FDL[numIRParts-1];
    for (u32 FDLIndex = numIRParts-1; FDLIndex > 0; FDLIndex--) {
        FDL[FDLIndex] = FDL[FDLIndex - 1];
    }
    FDL[0] = tempPtr;

    pffft_transform(fftSetup, fftTimeInputBuffer, FDL[0], nullptr, PFFFT_FORWARD);

    memset(convolutionDftResult, 0, dftSize * sizeof(float));
    
    for (u32 part_index = 0; part_index < numIRParts; part_index++) {
        pffft_zconvolve_accumulate(fftSetup,
                                   FDL[part_index],
                                   irDftBuffers[part_index],
                                   convolutionDftResult,
                                   1.0f);
    }
    
    pffft_transform(fftSetup, convolutionDftResult, fftTimeOutputBuffer, nullptr, PFFFT_BACKWARD);
    
    float fftSizeInv = 1.0f/(float)(fftSize);
    for (index = 0; index < nSamples; index++) {
        bufferL[index] = fftTimeOutputBuffer[fftSize - nSamples + index] * fftSizeInv;
    }

    
    // update IR here to make sure nothing changes during processing 
    if (updateIR) { 
        prepareConvolution(irBuffer, irBufferSize); 
    }
}

