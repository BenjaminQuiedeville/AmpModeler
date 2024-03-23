/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef TONE_STACK_H
#define TONE_STACK_H

#include "Biquad.h"


enum class ToneStackModel {
    EnglSavage = 0,
    JCM800,
    Soldano,
    Rectifier,
    Custom,
    N_MODELS
};

struct EQComponents {

    EQComponents(ToneStackModel ) {
        setModel(ToneStackModel::EnglSavage);
    }

    void setModel(ToneStackModel model) {
        
        
        switch (model) {
        case ToneStackModel::EnglSavage:

            R1 = 250e3;
            R2 = 1e6;
            R3 = 20e3;
            R4 = 47e3;
            C1 = 0.47e-9;
            C2 = 47e-9;
            C3 = 22e-9;    
            break;
        
        case ToneStackModel::JCM800:
            R1 = 220e3;
            R2 = 1e6;
            R3 = 22e3;
            R4 = 33e3;
            C1 = 0.47e-9;
            C2 = 22e-9;
            C3 = 22e-9;    
            break; 

        case ToneStackModel::Rectifier:
            R1 = 250e3;
            R2 = 1e6;
            R3 = 25e3;
            R4 = 47e3;
            C1 = 0.50e-9;
            C2 = 20e-9;
            C3 = 20e-9;    
            break; 

        case ToneStackModel::Soldano:
            R1 = 250e3;
            R2 = 1e6;
            R3 = 25e3;
            R4 = 47e3;
            C1 = 0.47e-9;
            C2 = 20e-9;
            C3 = 20e-9;    
            break; 

        case ToneStackModel::Custom:

            R1 = 250e3;
            R2 = 1e6;
            R3 = 20e3;
            R4 = 47e3;
            C1 = 0.47e-9;
            C2 = 47e-9;
            C3 = 22e-9;    
            break;

        }
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
        ToneStackModel model = ToneStackModel::EnglSavage;
        comp = new EQComponents(model);
    }

    ~Tonestack() {
        delete comp;
    }

    void prepareToPlay(double _samplerate) {

        updateCoefficients(0.5, 0.5, 0.5, _samplerate);

        x1 = 0.0;
        x2 = 0.0;
        x3 = 0.0;

        y1 = 0.0;
        y2 = 0.0;
        y3 = 0.0;
    }

    void updateCoefficients(float t, float m, float l, double samplerate);

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

    double b0 = 1.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double b3 = 0.0;

    double a1 = 0.0;
    double a2 = 0.0;
    double a3 = 0.0;

    sample_t x1 = 0.0;
    sample_t x2 = 0.0;
    sample_t x3 = 0.0;

    sample_t y1 = 0.0;
    sample_t y2 = 0.0;
    sample_t y3 = 0.0;

    EQComponents *comp;
};

#endif // TONE_STACK_H