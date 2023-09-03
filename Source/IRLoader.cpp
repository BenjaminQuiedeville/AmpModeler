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

// refaire l'impulse response avec des float et pas des doubles.
#define BASE_IR_SIZE 5793
static float baseIR[BASE_IR_SIZE] = {
#include "baseIR.inc"
};

IRLoader::IRLoader() {
    initIR = true;

    overlapAddIndex = 0;

}

IRLoader::~IRLoader() {

    if (fftSetup != nullptr) { pffft_destroy_setup(fftSetup); }
    pffft_aligned_free(irDftBuffer);
    pffft_aligned_free(inputBufferPadded);
    pffft_aligned_free(inputDftBuffer);
    pffft_aligned_free(convolutionResultBuffer);
    pffft_aligned_free(overlapAddBuffer);
}

void IRLoader::init(double _samplerate, int _blockSize) {

    blockSize = _blockSize;

    loadIR();
}

void IRLoader::prepareConvolution(const float *irPtr, int irSize) {

    int newfftSize = 1 << ((int)(log2(irSize - 1)) + 1);

    if (initIR) {
        fftSetup = pffft_new_setup(newfftSize, PFFFT_REAL);
        assert(fftSetup != 0);
        irDftBuffer             = (float *)pffft_aligned_malloc(2 * newfftSize * sizeof(float));
        inputBufferPadded       = (float *)pffft_aligned_malloc(newfftSize * sizeof(float));
        inputDftBuffer          = (float *)pffft_aligned_malloc(2 * newfftSize * sizeof(float));
        convolutionResultBuffer = (float *)pffft_aligned_malloc(2 * newfftSize * sizeof(float));
        overlapAddBuffer        = (float *)pffft_aligned_malloc(newfftSize * sizeof(float));

        memset(inputBufferPadded, 0, newfftSize * sizeof(float));
        memset(overlapAddBuffer,  0, newfftSize * sizeof(float));

        pffft_transform(fftSetup, irPtr, irDftBuffer, nullptr, PFFFT_FORWARD);

        return;
    }

    PFFFT_Setup *fftSetupOld          = fftSetup;
    float *irDftBufferOld             = irDftBuffer;
    float *inputBufferPaddedOld       = inputBufferPadded;
    float *inputDftBufferOld          = inputDftBuffer;
    float *convolutionResultBufferOld = convolutionResultBuffer;
    float *overlapAddBufferOld        = overlapAddBuffer;

    PFFFT_Setup *fftSetupTemp = pffft_new_setup(newfftSize, PFFFT_REAL);
    assert(fftSetup != 0);
    float *irDftBufferTemp             = (float *)pffft_aligned_malloc(2 * newfftSize * sizeof(float));
    float *inputBufferPaddedTemp       = (float *)pffft_aligned_malloc(newfftSize * sizeof(float));
    float *inputDftBufferTemp          = (float *)pffft_aligned_malloc(2 * newfftSize * sizeof(float));
    float *convolutionResultBufferTemp = (float *)pffft_aligned_malloc(2 * newfftSize * sizeof(float));
    float *overlapAddBufferTemp        = (float *)pffft_aligned_malloc(newfftSize * sizeof(float));

    pffft_transform(fftSetupTemp, irPtr, irDftBufferTemp, nullptr, PFFFT_FORWARD);

    irDftBuffer             = irDftBufferTemp;
    inputBufferPadded       = inputBufferPaddedTemp;
    inputDftBuffer          = inputDftBufferTemp;
    convolutionResultBuffer = convolutionResultBufferTemp;
    overlapAddBuffer        = overlapAddBufferTemp;

    convolutionResultSize = irSize + blockSize - 1;

    pffft_destroy_setup(fftSetupOld);
    pffft_aligned_free(irDftBufferOld);
    pffft_aligned_free(inputBufferPaddedOld);
    pffft_aligned_free(inputDftBufferOld);
    pffft_aligned_free(convolutionResultBufferOld);
    pffft_aligned_free(overlapAddBufferOld);

    fftSize = newfftSize;
}

void IRLoader::loadIR() {

    if (initIR) {
        prepareConvolution(baseIR, BASE_IR_SIZE);
        initIR = false;
        return;
    }

    auto irChooser = std::make_unique<juce::FileChooser>("Select an .wav IR to load",
                                                          juce::File{},
                                                          "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    auto formatManager = new juce::AudioFormatManager();
    juce::AudioBuffer<float> *irBuffer;

    irChooser->launchAsync(chooserFlags, [&, this](const juce::FileChooser& fc) {
        auto file = fc.getResult();

        if (file != juce::File{}) {
            auto reader = formatManager->createReaderFor (file);

            if (reader != nullptr) {
                irBuffer = new juce::AudioBuffer<float>(1, reader->lengthInSamples);
                reader->read(irBuffer, 0, reader->lengthInSamples, 0, true, false);
            }
        }
    });
    prepareConvolution(irBuffer->getReadPointer(0), irBuffer->getNumSamples());

    delete formatManager;
    delete irBuffer;
}


void IRLoader::process(float *input, size_t nSamples) {

    for (size_t i = 0; i < nSamples; i++) {
        inputBufferPadded[i] = input[i];
    }


    pffft_transform(fftSetup, inputBufferPadded, inputDftBuffer, nullptr, PFFFT_FORWARD);

    pffft_zconvolve_accumulate(fftSetup, inputDftBuffer, irDftBuffer, convolutionResultBuffer, 1/fftSize);

    pffft_transform(fftSetup, convolutionResultBuffer, inputBufferPadded, nullptr, PFFFT_BACKWARD);

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

    overlapAddIndex = (overlapAddIndex + nSamples) % (fftSize);
}
