/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef TONE_STACK_H
#define TONE_STACK_H

#include "Biquad.h"


enum TonestackModel {
    EnglSavage = 0,
    JCM800,
    Soldano,
    Rectifier,
    Orange,
    Custom,
    N_MODELS
};

struct TonestackConstants {
    
    double beta11;
    double beta12;
    double beta13;
    double beta14;
    
    double beta21;
    double beta22;
    double beta23;
    double beta24;
    double beta25;
    double beta26;
    
    double beta31;
    double beta32;
    double beta33;
    double beta34;
    double beta35;
    double beta36;

    double alpha11;
    double alpha12;
    double alpha13;
    
    double alpha21;
    double alpha22;
    double alpha23;
    double alpha24;
    double alpha25;
    
    double alpha31;
    double alpha32;
    double alpha33;
    double alpha34;
    double alpha35;

};


struct TonestackComponents {

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
    
        comps = new TonestackComponents();
        ctes  = new TonestackConstants();
        setModel(EnglSavage);
    }

    ~Tonestack() {
        delete comps;
        delete ctes;
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


    void setModel(TonestackModel newModel);

    void updateConstants();
    
    void updateCoefficients(float t, float m, float l, double samplerate);

    void process(sample_t *buffer, size_t nSamples) { 
       
        for (size_t i = 0; i < nSamples; i++) {
        
            sample_t outputSample = (sample_t)(buffer[i] * b0
                                  + x1 * b1
                                  + x2 * b2
                                  + x3 * b3
                                  - y1 * a1
                                  - y2 * a2
                                  - y3 * a3);

            x3 = x2; 
            x2 = x1;
            x1 = buffer[i];
            
            y3 = y2; 
            y2 = y1;
            y1 = outputSample;

            buffer[i] = outputSample;
        }
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

    TonestackModel model;
    TonestackComponents *comps;

    TonestackConstants *ctes;
};

#endif // TONE_STACK_H