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
    
    pffft_aligned_free(fftSignalsMemory);
    free(ptrBuffersMemory);
    free(ir1.buffer);
    free(ir2.buffer);
}

IRLoader::~IRLoader() {
    deallocateFFTEngine();
    
    if (irBuffer) { free(irBuffer); }
}


void IRLoader::init(double _samplerate, size_t _blockSize) {
    samplerate = _samplerate;
    blockSize = _blockSize;

    loadIR(1);
    loadIR(2);
}

void IRLoader::prepareConvolution(float *irPtr, size_t irSize) {
    // recharger les deux IR à chaque fois, ca prend plus de temps mais y'a moins de dispatch à faire 
    // On rallonge la FDL en fonction de l'IR la plus longue et on arrête les calculs en cours de route pour la petite IR
    // On applique le déphasage juste avant de faire zconvolve
    // Comme chaque segment d'IR sera déphasé de la même manière, on peut précalculer les valeurs à appliquer et juste faire les multiplications
    if (irToUpdate == 1) {
    
    } else if (irToUpdate == 2) {
    
    } else {
        assert(false && "prepareConvolution: wrong ir index");
    }
    
    numIRParts = (u32)(irSize / blockSize);
    if (irSize % blockSize != 0) { numIRParts += 1; } 

    assert(nextPowTwo(blockSize) == blockSize && "IRLoader, blockSize must be a power of 2!");
    
    fftSize = 2*blockSize;
    dftSize = fftSize + 2;
    deallocateFFTEngine();
    
    fftSetup = pffft_new_setup((int)fftSize, PFFFT_REAL);
    
    // we need to align to 64 bytes any buffer that will go through the fft;
    ptrBuffersMemory = (float**)calloc(numIRParts*3, sizeof(float*));
    irDftBuffers = ptrBuffersMemory;
    // FDLLeft = (float**)calloc(numIRPar )
    
    ptrBuffersMemory + numIRParts;
    FDLRight = FDLLeft + numIRParts;
    
    u32 alignment = 64;
    u64 partAllocSize = dftSize * sizeof(float);
    partAllocSize += alignment - (partAllocSize & (alignment-1));
    
    u64 fftAllocSize = fftSize * sizeof(float);
    fftAllocSize += alignment - (fftAllocSize & (alignment-1));
    
    u64 memorySize = numIRParts * partAllocSize
                    + numIRParts * partAllocSize
                    + numIRParts * partAllocSize
                    + partAllocSize 
                    + fftAllocSize
                    + fftAllocSize
                    + fftAllocSize;
                    
    fftSignalsMemory = (float*)pffft_aligned_malloc(memorySize);
    memset(fftSignalsMemory, 0, memorySize);
    assert(((uintptr_t)fftSignalsMemory & (alignment-1)) == 0);


    u64 memoryIndex = 0;
    for (u32 index = 0; index < numIRParts; index++) {
        irDftBuffers[index] = (float*)(fftSignalsMemory + memoryIndex);    
        memoryIndex += partAllocSize / sizeof(float);
        assert(((uintptr_t)irDftBuffers[index] & (alignment-1)) == 0);
    }
    
    for (u32 index = 0; index < numIRParts; index++) {
        FDLLeft[index] = (float*)(fftSignalsMemory + memoryIndex);    
        memoryIndex += partAllocSize / sizeof(float);
        assert(((uintptr_t)FDLLeft[index] & (alignment-1)) == 0);
    }
    
    for (u32 index = 0; index < numIRParts; index++) {
        FDLRight[index] = (float*)(fftSignalsMemory + memoryIndex);    
        memoryIndex += partAllocSize / sizeof(float);
        assert(((uintptr_t)FDLLeft[index] & (alignment-1)) == 0);
    }
    
    convolutionDftResult = (float*)(fftSignalsMemory + memoryIndex);   
    memoryIndex += partAllocSize / sizeof(float);
    assert(((uintptr_t)convolutionDftResult & (alignment-1)) == 0);

    fftTimeInputBufferLeft = (float*)(fftSignalsMemory + memoryIndex);
    memoryIndex += fftAllocSize / sizeof(float);
    assert(((uintptr_t)fftTimeInputBufferLeft & (alignment-1)) == 0);
    
    fftTimeInputBufferRight = (float*)(fftSignalsMemory + memoryIndex);
    memoryIndex += fftAllocSize / sizeof(float);
    assert(((uintptr_t)fftTimeInputBufferRight & (alignment-1)) == 0);
    
    fftTimeOutputBuffer = (float*)(fftSignalsMemory + memoryIndex);
    memoryIndex += fftAllocSize / sizeof(float);
    assert(((uintptr_t)fftTimeOutputBuffer & (alignment-1)) == 0);
    
    assert(memoryIndex == memorySize/4);
    
    u32 baseIRIndex = 0;
    u32 partSize = 0;
    for (u32 index = 0; index < numIRParts; index++) {
        baseIRIndex = (u32)(index * blockSize);
        
        if (baseIRIndex + blockSize >= irSize) {
            partSize = (u32)(irSize - baseIRIndex);    
        } else {
            partSize = (u32)blockSize;
        }
        
        // memset(fftTimeInputBuffer, 0, fftAllocSize);
        for (u32 timeBufferIndex = 0; timeBufferIndex < fftSize; timeBufferIndex++) {
            fftTimeInputBufferLeft[timeBufferIndex] = 0.0f;
        }
        memcpy(fftTimeInputBufferLeft, &irPtr[baseIRIndex], partSize * sizeof(float));
        
        pffft_transform(fftSetup, fftTimeInputBufferLeft, irDftBuffers[index], nullptr, PFFFT_FORWARD);
    }

    memset(fftTimeInputBufferLeft, 0, fftAllocSize);
    memset(fftTimeInputBufferRight, 0, fftAllocSize);
    memset(fftTimeOutputBuffer, 0, fftAllocSize);
    
    irToUpdate = 0;
}

IRLoaderError IRLoader::loadIR(int whichIR) {
    juce::File* currentFile = nullptr;
    float *irBuffer = nullptr;
    u32 *irSize = nullptr;

    // initialiser l'IR2 sur la default aussi, en bypass pour sauver du process
    // ca permet de sauver du dispatch
    
    if (whichIR == 1) {
        currentFile = &irFile1;
        irBuffer = ir1.buffer;
        irSize = &ir1.size;
        
    } else if (whichIR == 2) {
        
        currentFile = &irFile2;
        irBuffer = ir2.buffer;
        irSize = &ir2.size;
        
    } else {
        assert(false && "LoadIR: wrong ir index");
    }
    
    if (defaultIR) {        
        if (samplerate == 44100.0) {
            irBuffer = baseIR_441;
            *irSize = base_IR_441_SIZE;            
            prepareConvolution();
        } else if (samplerate == 48000.0) {
            irBuffer = baseIR_48;
            *irSize = base_IR_48_SIZE;            
            prepareConvolution();
        } else {
            juce::AlertWindow::showMessageBox(
                juce::MessageBoxIconType::WarningIcon, 
                "Samplerate error",
                "Sorry, the default provided IR is only available at 44.1 or 48kHz, if you work at another sampling rate, please provide your own IR.", 
                "Ok");        
            return IRLoaderError::Error;
        }
        reloadIR = true;
        return IRLoaderError::OK;
    }
        
    std::string irPath = currentFile->getFullPathName().toStdString();
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
        
        free(irBuffer);        
        irBuffer = (float*)calloc(wav.totalPCMFrameCount, sizeof(float));
        numFramesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, irBuffer);
        
        assert(numFramesRead == wav.totalPCMFrameCount && "Error in decoding the wav file");
        *irSize = (u32)numFramesRead;

        wav_opening_scope_end: {
            drwav_uninit(&wav);
        }
    }
    prepareConvolution();
    return error;
}


void IRLoader::process(float *bufferL, float *bufferR, size_t nSamples) {
    ZoneScoped;
    
    float fftSizeInv = 1.0f/(float)(fftSize);

    for (u32 index = 0; index < fftSize-nSamples; index++) {
        fftTimeInputBufferLeft[index] = fftTimeInputBufferLeft[index + nSamples];
    }
    
    for (u32 index = 0; index < nSamples; index++) {
        fftTimeInputBufferLeft[fftSize-nSamples + index] = bufferL[index];
    }
        
    
    //shift the FDL
    float* tempPtr = FDLLeft[numIRParts-1];
    for (u32 FDLIndex = numIRParts-1; FDLIndex > 0; FDLIndex--) {
        FDLLeft[FDLIndex] = FDLLeft[FDLIndex - 1];
    }
    FDLLeft[0] = tempPtr;

    pffft_transform(fftSetup, fftTimeInputBufferLeft, FDLLeft[0], nullptr, PFFFT_FORWARD);

    memset(convolutionDftResult, 0, dftSize * sizeof(float));
    
    for (u32 part_index = 0; part_index < numIRPartsMax; part_index++) {
        
        if part_index > numIRParts1 {break;}
    
        pffft_zconvolve_accumulate(fftSetup,
                                   FDLLeft[part_index],
                                   irDftBuffers[part_index],
                                   convolutionDftResult,
                                   1.0f);
    }
    
    pffft_transform(fftSetup, convolutionDftResult, fftTimeOutputBuffer, nullptr, PFFFT_BACKWARD);
    
    for (u32 index = 0; index < nSamples; index++) {
        bufferL[index] = fftTimeOutputBuffer[fftSize - nSamples + index] * fftSizeInv;
    }

    if (bufferR) {    
        for (u32 index = 0; index < fftSize-nSamples; index++) {
            fftTimeInputBufferRight[index] = fftTimeInputBufferRight[index + nSamples];
        }
        
        for (u32 index = 0; index < nSamples; index++) {
            fftTimeInputBufferRight[fftSize-nSamples + index] = bufferR[index];
        }
            
        
        //shift the FDL
        tempPtr = FDLRight[numIRParts-1];
        for (u32 FDLIndex = numIRParts-1; FDLIndex > 0; FDLIndex--) {
            FDLRight[FDLIndex] = FDLRight[FDLIndex - 1];
        }
        FDLRight[0] = tempPtr;
    
        pffft_transform(fftSetup, fftTimeInputBufferRight, FDLRight[0], nullptr, PFFFT_FORWARD);
    
        memset(convolutionDftResult, 0, dftSize * sizeof(float));
        
        for (u32 part_index = 0; part_index < numIRParts; part_index++) {
            pffft_zconvolve_accumulate(fftSetup,
                                       FDLRight[part_index],
                                       irDftBuffers[part_index],
                                       convolutionDftResult,
                                       1.0f);
        }
        
        pffft_transform(fftSetup, convolutionDftResult, fftTimeOutputBuffer, nullptr, PFFFT_BACKWARD);
        
        for (u32 index = 0; index < nSamples; index++) {
            bufferR[index] = fftTimeOutputBuffer[fftSize - nSamples + index] * fftSizeInv;
        }
    }
}

