/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef TONE_STACK_H
#define TONE_STACK_H

#include "common.h"

enum TonestackModel {
    Soldano = 0,
    EnglSavage,
    JCM800,
    // Rectifier,
    // Orange,
    // Custom,
    N_MODELS
};

struct Tonestack {

    Tonestack() {    
        setModel(EnglSavage);
    }


    void prepareToPlay(double _samplerate) {

        updateCoefficients(0.5, 0.5, 0.5, _samplerate);

        x1L = 0.0f;
        x2L = 0.0f;
        x3L = 0.0f;
        y1L = 0.0f;
        y2L = 0.0f;
        y3L = 0.0f;
        
        x1R = 0.0f;
        x2R = 0.0f;
        x3R = 0.0f;
        y1R = 0.0f;
        y2R = 0.0f;
        y3R = 0.0f;
    }


    void setModel(TonestackModel newModel);

    void updateConstants();
    
    void updateCoefficients(float t, float m, float l, double samplerate);
    
    void process(Sample *bufferL, Sample *bufferR, size_t nSamples) {
        if (bufferL) {
            for (size_t i = 0; i < nSamples; i++) {
            
                Sample outputSample = (Sample)(bufferL[i] * b0
                                    + x1L * b1
                                    + x2L * b2
                                    + x3L * b3
                                    - y1L * a1
                                    - y2L * a2
                                    - y3L * a3);
    
                x3L = x2L; 
                x2L = x1L;
                x1L = bufferL[i];
                
                y3L = y2L; 
                y2L = y1L;
                y1L = outputSample;
    
                bufferL[i] = outputSample;
            }
        }
        
        if (bufferR) {    
            for (size_t i = 0; i < nSamples; i++) {
                
                Sample outputSample = (Sample)(bufferR[i] * b0
                                    + x1R * b1
                                    + x2R * b2
                                    + x3R * b3
                                    - y1R * a1
                                    - y2R * a2
                                    - y3R * a3);
    
                x3R = x2R; 
                x2R = x1R;
                x1R = bufferR[i];
                
                y3R = y2R; 
                y2R = y1R;
                y1R = outputSample;
    
                bufferR[i] = outputSample;
            }
        }           
    }

    double b0 = 1.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double b3 = 0.0;

    double a1 = 0.0;
    double a2 = 0.0;
    double a3 = 0.0;

    Sample x1L = 0.0f;
    Sample x2L = 0.0f;
    Sample x3L = 0.0f;

    Sample y1L = 0.0f;
    Sample y2L = 0.0f;
    Sample y3L = 0.0f;

    Sample x1R = 0.0f;
    Sample x2R = 0.0f;
    Sample x3R = 0.0f;

    Sample y1R = 0.0f;
    Sample y2R = 0.0f;
    Sample y3R = 0.0f;

    TonestackModel model;
    
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
    } ctes;

    struct TonestackComponents {
    
        double R1 = 0.0;
        double R2 = 0.0;
        double R3 = 0.0;
        double R4 = 0.0;
    
        double C1 = 0.0;
        double C2 = 0.0;
        double C3 = 0.0;
    } comps;
};

#endif // TONE_STACK_H
