/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef TONE_STACK_H
#define TONE_STACK_H

#include "common.h"

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
    
    double beta11 = 0.0;
    double beta12 = 0.0;
    double beta13 = 0.0;
    double beta14 = 0.0;
    
    double beta21 = 0.0;
    double beta22 = 0.0;
    double beta23 = 0.0;
    double beta24 = 0.0;
    double beta25 = 0.0;
    double beta26 = 0.0;
    
    double beta31 = 0.0;
    double beta32 = 0.0;
    double beta33 = 0.0;
    double beta34 = 0.0;
    double beta35 = 0.0;
    double beta36 = 0.0;

    double alpha11 = 0.0;
    double alpha12 = 0.0;
    double alpha13 = 0.0;
    
    double alpha21 = 0.0;
    double alpha22 = 0.0;
    double alpha23 = 0.0;
    double alpha24 = 0.0;
    double alpha25 = 0.0;
    
    double alpha31 = 0.0;
    double alpha32 = 0.0;
    double alpha33 = 0.0;
    double alpha34 = 0.0;
    double alpha35 = 0.0;
};


struct TonestackComponents {

    double R1 = 0.0;
    double R2 = 0.0;
    double R3 = 0.0;
    double R4 = 0.0;

    double C1 = 0.0;
    double C2 = 0.0;
    double C3 = 0.0;
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

    __forceinline void process(Sample *buffer, size_t nSamples) {
       
        for (size_t i = 0; i < nSamples; i++) {
        
            Sample outputSample = (Sample)(buffer[i] * b0
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

    Sample x1 = 0.0;
    Sample x2 = 0.0;
    Sample x3 = 0.0;

    Sample y1 = 0.0;
    Sample y2 = 0.0;
    Sample y3 = 0.0;

    TonestackModel model;
    TonestackComponents *comps;

    TonestackConstants *ctes;
};

#endif // TONE_STACK_H