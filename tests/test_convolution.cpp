/*
vérifier le bon fonctionnement sur un bruit blanc de 10 sec
générer un bruit blanc d'une minute
faire la convolution OLA et la UPOLS avec les IRs de bases
*/

// compilation command 
// cl /Fe:test_convolution.exe /EHsc /Ox test_convolution.cpp ../libs/pffft/pffft.c && test_convolution.exe 

#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <memory>
#include <random>
#include <chrono>
#include "cassert"
#include "../Source/common.h"
#include "../libs/pffft/pffft.h"

#define BASE_IR_SIZE 1323
static float baseIR[BASE_IR_SIZE] = {
#include "../Source/data/baseIR.inc"
};


struct OLAConvolution {

    void process(float *bufferL, float *bufferR, size_t nSamples) {
        
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
    }

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


struct UPOLSConvolution {

    float **irDftBuffers = nullptr;
    float **FDL = nullptr;
    float *convolutionDftResult = nullptr;
    float *fftTimeBuffer;
    u32 numIRParts = 0;
    u32 dftSize = 0;
    u64 fftSize = 0;
    u32 irSize = 0;
    PFFFT_Setup *fftSetup = nullptr;
};


void UPOLS_process(UPOLSConvolution *conv, float *bufferL, u32 nSamples) {
    memset(conv->fftTimeBuffer, 0, conv->fftSize * sizeof(float));
    memcpy(conv->fftTimeBuffer, bufferL, nSamples*sizeof(float));


    //shift the FDL
    float* tempPtr = conv->FDL[conv->numIRParts-1];
    for (u32 FDLIndex = conv->numIRParts-1; FDLIndex > 0; FDLIndex--) {
        conv->FDL[FDLIndex] = conv->FDL[FDLIndex - 1];
    }
    conv->FDL[0] = tempPtr;

    pffft_transform(conv->fftSetup, conv->fftTimeBuffer, conv->FDL[0], nullptr, PFFFT_FORWARD);

    memset(conv->convolutionDftResult, 0, conv->dftSize * sizeof(float));
    
    for (u32 part_index = 0; part_index < conv->numIRParts; part_index++) {
        pffft_zconvolve_accumulate(conv->fftSetup, 
                                   conv->FDL[part_index], 
                                   conv->irDftBuffers[part_index], 
                                   conv->convolutionDftResult, 
                                   1.0);
    }
    
    pffft_transform(conv->fftSetup, conv->convolutionDftResult, conv->fftTimeBuffer, nullptr, PFFFT_BACKWARD);

    memcpy(bufferL, conv->fftTimeBuffer, nSamples*sizeof(float));

    for (u32 sample_index = 0; sample_index < nSamples; sample_index++) {
        bufferL[sample_index] *= 1.0f/(float)(conv->fftSize);
    }
}

void test_UPOLS(double samplerate, double length_s, u64 bufferSize) {    
        
    UPOLSConvolution conv;
    u64 lengthSamples = (u64)(length_s * samplerate);
    conv.fftSize = 2 * bufferSize;
    conv.irSize = BASE_IR_SIZE;
    conv.dftSize = bufferSize*2 + 2;
    
    // baseIR
    conv.numIRParts = conv.irSize / bufferSize;
    if (conv.irSize % bufferSize != 0) { conv.numIRParts += 1; } 
    
    
    {
        u64 part_size_bytes = conv.dftSize * sizeof(float);
        u64 padding = part_size_bytes & 0xF; 
        part_size_bytes += padding;
        
        conv.irDftBuffers = (float**)calloc(conv.numIRParts, sizeof(float*));
        conv.irDftBuffers[0] = (float*)pffft_aligned_malloc(conv.numIRParts * (part_size_bytes));
        
        conv.FDL = (float**)calloc(conv.numIRParts, sizeof(float));
        conv.FDL[0] = (float*)pffft_aligned_malloc(conv.numIRParts * (part_size_bytes));

        conv.convolutionDftResult = (float*)pffft_aligned_malloc(part_size_bytes);

    
        //memcpy(dest, source, taille_bytes)
        
        for (u32 index = 1; index < conv.numIRParts; index++) {
            conv.irDftBuffers[index] = (float*)((uintptr_t)conv.irDftBuffers[index-1] + part_size_bytes);
            conv.FDL[index]          = (float*)((uintptr_t)conv.FDL[index-1] + part_size_bytes);
        }
    }
    
    conv.fftSetup = pffft_new_setup((int)conv.fftSize, PFFFT_REAL);
    
    u32 baseIRIndex = 0;
    u32 partSize = 0;
    conv.fftTimeBuffer = (float*)pffft_aligned_malloc(conv.fftSize * sizeof(float));
    
    for (u32 index = 0; index < conv.numIRParts; index++) {
        baseIRIndex = index * bufferSize;
        
        if (baseIRIndex + bufferSize >= conv.irSize) {
            partSize = conv.irSize - baseIRIndex;    
        } else {
            partSize = bufferSize;
        }
        
        memset(conv.fftTimeBuffer, 0, conv.fftSize * sizeof(float));
        memcpy(conv.fftTimeBuffer, &baseIR[baseIRIndex], partSize * sizeof(float));
        
        pffft_transform(conv.fftSetup, conv.fftTimeBuffer, conv.irDftBuffers[index], nullptr, PFFFT_FORWARD);
    }

    float *input_blockL = (float*)calloc(bufferSize, sizeof(float));

    for (u32 i = 0; i < bufferSize; i++) {
        input_blockL[i] = (float)rand()/RAND_MAX * 2.0f - 1.0;
    }


    std::cout << "Testing UPOLS algorithm, parameters : \n"
              << "samplerate : " << samplerate << " Hz\n"
              << "block size : " << bufferSize << "\n"
              << "duration : "   << length_s << " s \n"       
              << std::endl;

    std::ofstream output_file { "UPOLS_output.txt" };
    auto iteration_begin = std::chrono::high_resolution_clock::now();    
    auto iteration_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_accumulator; 
    
    // processing
    for (u64 samples_processed = 0; samples_processed < lengthSamples; samples_processed+=bufferSize) {
        
        iteration_begin = std::chrono::high_resolution_clock::now();
        UPOLS_process(&conv, input_blockL, bufferSize);
        iteration_end = std::chrono::high_resolution_clock::now();
        time_accumulator += iteration_end - iteration_begin;
    
        for (u32 sample_index = 0; sample_index < bufferSize; sample_index++) {
            output_file << input_blockL[sample_index] << "\n";
            input_blockL[sample_index] = (float)rand()/RAND_MAX * 2.0f - 1.0f;
        }
    }

    float num_blocks = (float)(lengthSamples / bufferSize);

    std::cout << "time elapsed : " << time_accumulator.count() * 1000.0 << " ms\n"
              << "time per mono block : " << (time_accumulator.count() / (num_blocks*2)) * 1000.0 << " ms"  
              << std::endl;

    
    pffft_aligned_free(conv.irDftBuffers[0]);
    pffft_aligned_free(conv.FDL[0]);
    free(conv.irDftBuffers);
    free(conv.FDL);

    pffft_aligned_free(conv.convolutionDftResult);
    pffft_aligned_free(conv.fftTimeBuffer);

    pffft_destroy_setup(conv.fftSetup);
    
}

void test_OLA(double samplerate, double length_s, u64 buffer_size) {
    
    OLAConvolution conv;
    
    u64 length_samples = (u64)(length_s * samplerate);

    u64 irSize = BASE_IR_SIZE;
    conv.irBufferSize = irSize;

    u64 fftSize = nextPowTwo(irSize + 100);
    conv.fftSize = fftSize;
    conv.samplerate = samplerate;
    conv.blockSize = buffer_size;
    conv.fftEngine = pffft_new_setup((int)fftSize, PFFFT_REAL);

    conv.inputBufferPadded           = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    conv.inputDftBuffer              = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    conv.irDftBuffer                 = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    conv.convolutionResultBuffer     = (float *)pffft_aligned_malloc(2 * fftSize * sizeof(float));
    conv.convolutionResultDftBuffer  = (float *)pffft_aligned_malloc((fftSize + 2) * sizeof(float));
    conv.overlapAddBufferL           = (float *)calloc(2 * fftSize, sizeof(float));
    conv.overlapAddBufferR           = (float *)calloc(2 * fftSize, sizeof(float));
    
    if (fftSize >= 16384) {
        conv.fftWorkBuffer = (float *)pffft_aligned_malloc(fftSize * sizeof(float));
    } else {
        conv.fftWorkBuffer = nullptr;
    }
    
    memset(conv.inputBufferPadded, 0, fftSize * sizeof(float));

    float *irTimeVec = (float *)pffft_aligned_malloc(fftSize * sizeof(float));

    memcpy(irTimeVec, baseIR, irSize * sizeof(float));
    memset(irTimeVec+irSize, 0, (fftSize - irSize)*sizeof(float));

    pffft_transform(conv.fftEngine, irTimeVec, conv.irDftBuffer, conv.fftWorkBuffer, PFFFT_FORWARD);

    conv.convolutionResultSize = irSize + conv.blockSize - 1;

    pffft_aligned_free(irTimeVec);


    // process
    std::cout << "Testing OLA algorithm, parameters : \n"
              << "samplerate : " << samplerate << " Hz\n"
              << "block size : " << conv.blockSize << "\n"
              << "duration : "   << length_s << " s \n"       
              << std::endl;
    
    float *input_blockL = (float*)calloc(conv.blockSize, sizeof(float));
    float *input_blockR = (float*)calloc(conv.blockSize, sizeof(float));

    for (u32 i = 0; i < conv.blockSize; i++) {
        input_blockL[i] = (float)rand()/RAND_MAX * 2.0f - 1.0;
        input_blockR[i] = (float)rand()/RAND_MAX * 2.0f - 1.0;
    }

        
    auto start_time = std::chrono::high_resolution_clock::now();
        
    for (u64 samples_processed = 0; samples_processed < length_samples; samples_processed+=buffer_size) {    
        conv.process(input_blockL, input_blockR, conv.blockSize);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_elapsed = (end_time - start_time);

    float num_blocks = (float)(length_samples / buffer_size);


    std::cout << "time elapsed : " << time_elapsed.count() * 1000.0 << " ms \n"
              << "time per mono block : " << (time_elapsed.count() / (num_blocks*2)) * 1000.0 << " ms"  << std::endl;

    
    if (conv.fftEngine) {
        pffft_destroy_setup(conv.fftEngine);
    }
    
    pffft_aligned_free(conv.inputBufferPadded);
    pffft_aligned_free(conv.inputDftBuffer);
    pffft_aligned_free(conv.irDftBuffer);
    pffft_aligned_free(conv.convolutionResultBuffer);
    pffft_aligned_free(conv.convolutionResultDftBuffer);
    pffft_aligned_free(conv.fftWorkBuffer);
    free(conv.overlapAddBufferL);
    free(conv.overlapAddBufferR);

    free(input_blockL);
    free(input_blockR);
}


int main(int argc, char **argv) {
    
    printf("\n\n");

    double samplerate = 44100.0;
    double length_s = 60.0;
    u64 bufferSize = 128;


    test_OLA(samplerate, length_s, bufferSize);
    printf("\n---------------------------------------\n\n");
    test_UPOLS(samplerate, length_s, bufferSize);

    return 0;
}
