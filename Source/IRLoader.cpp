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
    
    memset(inputBufferPadded, 0, fftSize * sizeof(float));
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

    memcpy(irTimeVec, irPtr, irSize * sizeof(float));
    memset(irTimeVec+irSize, 0, (fftSize - irSize)*sizeof(float));

    pffft_transform(fftEngine, irTimeVec, irDftBuffer, fftWorkBuffer, PFFFT_FORWARD);

    convolutionResultSize = irSize + blockSize - 1;
    updateIR = false;

    pffft_aligned_free(irTimeVec);

    return;
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
        if (!drwav_init_file(&wav, irPath.data(), NULL)) {
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
    
        if (wav.sampleRate != 44100 && wav.sampleRate != 48000) {
            juce::AlertWindow::showMessageBox(
                juce::MessageBoxIconType::WarningIcon, 
                "Samplerate error",
                "Sorry, the default provided IR is only available at 44.1 or 48kHz, if you work at another sampling rate, please provide your own IR.", 
                "Ok");
            error = IRLoaderError::Error;
            goto wav_opening_scope_end;
        }
    
        irBuffer = (float*)calloc(wav.totalPCMFrameCount, sizeof(float));
        numFramesRead = drwav_read_pcm_frames_f32(&wav, wav.totalPCMFrameCount, irBuffer);
    
        if (numFramesRead != wav.totalPCMFrameCount) { return IRLoaderError::Error; }
        irBufferSize = numFramesRead;

        wav_opening_scope_end: {
            drwav_uninit(&wav);
        }
    }

    // the IR is fully loaded at the end of the process function
    updateIR = true;
    
    return error;
}


void IRLoader::process(float *bufferL, float *bufferR, size_t nSamples) {
    
    memcpy(inputBufferPadded, bufferL, nSamples * sizeof(float));
 
    pffft_transform(fftEngine, inputBufferPadded, inputDftBuffer, fftWorkBuffer, PFFFT_FORWARD);
    
    memset(convolutionResultDftBuffer, 0, (fftSize+2)*sizeof(Sample));

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
    
        memcpy(inputBufferPadded, bufferR, nSamples * sizeof(float));
    
        pffft_transform(fftEngine, inputBufferPadded, inputDftBuffer, fftWorkBuffer, PFFFT_FORWARD);
    
        memset(convolutionResultDftBuffer, 0, (fftSize+2)*sizeof(Sample));
    
        pffft_zconvolve_accumulate(fftEngine, inputDftBuffer, irDftBuffer, convolutionResultDftBuffer, 1.0f);
    
        pffft_transform(fftEngine, convolutionResultDftBuffer, convolutionResultBuffer, fftWorkBuffer, PFFFT_BACKWARD);
    
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
}

