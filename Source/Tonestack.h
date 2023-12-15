/*
  ==============================================================================

    ThreeBandEQ.h
    Created: 23 Apr 2023 9:08:03pm
    Author:  Benjamin

  ==============================================================================
*/

#ifndef TONE_STACK_H
#define TONE_STACK_H

#include "JuceHeader.h"
#include "Biquad.h"

#define RESONANCE_FREQUENCY 250.0
#define RESONANCE_Q         0.7
#define PRESENCE_FREQUENCY  500.0
#define PRESENCE_Q          0.4


struct EQComponents {

    EQComponents() {
        // ajouter plus de choix pour les composants du toneStack

        // Composants du Soldano SLO
        R1 = 250e3;
        R2 = 1e6;
        R3 = 25e3;
        R4 = 47e3;
        C1 = 0.47e-9;
        C2 = 20e-9;
        C3 = 20e-9;    
    }

    double R1;
    double R2;
    double R3;
    double R4;

    double C1;
    double C2;
    double C3;
};


struct Tonestack {

    Tonestack() {
        comp = new EQComponents();
    }

    ~Tonestack() {
        delete comp;
    }

    void prepareToPlay(double _samplerate) {

        samplerate = _samplerate;
        updateCoefficients(0.5, 0.5, 0.5);

        x1 = 0.0;
        x2 = 0.0;
        x3 = 0.0;

        y1 = 0.0;
        y2 = 0.0;
        y3 = 0.0;
    }

    void updateCoefficients(float t, float m, float l);

    void process(sample_t *signal, size_t nSamples) {
        for (size_t i = 0; i < nSamples; i++) {
           signal[i] = processSample(signal[i]);
        }
    }

    inline sample_t processSample(sample_t sample) {

        sample_t outputSample = (float)(sample * b0
                            + x1 * b1
                            + x2 * b2
                            + x3 * b3
                            - y1 * a1
                            - y2 * a2
                            - y3 * a3);

        x3 = x2; 
        x2 = x1;
        x1 = sample;
        
        y3 = y2; 
        y2 = y1;
        y1 = outputSample;

        return outputSample;
    }

    double b0;
    double b1;
    double b2;
    double b3;

    double a1;
    double a2;
    double a3;

    sample_t x1;
    sample_t x2;
    sample_t x3;

    sample_t y1;
    sample_t y2;
    sample_t y3;

    EQComponents *comp;
    double samplerate;
};

#endif // TONE_STACK_H