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
}
