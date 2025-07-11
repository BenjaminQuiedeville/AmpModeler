/*
==============================================================================
    Author:  Benjamin Quiedeville
==============================================================================
*/

#ifndef BIQUAD_H
#define BIQUAD_H

#include "common.h"

enum FilterType : u8 {
    BIQUAD_LOWPASS = 0,
    BIQUAD_HIGHPASS,
    BIQUAD_PEAK,
    BIQUAD_LOWSHELF,
    BIQUAD_HIGHSHELF,
    BIQUAD_NFILTERTYPES,
};

//@TODO refactor to be thread safe
struct Biquad {

    Biquad(FilterType type) { filterType = type; }

    void prepareToPlay() {

        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
        a1 = 0.0;
        a2 = 0.0;
        reset();
    }

    void reset() {
        w1L = 0.0f;
        w2L = 0.0f;
        w1R = 0.0f;
        w2R = 0.0f;
    }

    void setCoefficients(double frequency, double Q, double gaindB, double samplerate) {
        ZoneScoped;
            
        double w0 = 2 * M_PI / samplerate * frequency;
        double cosw0 = cos(w0);
        double sinw0 = sin(w0);
    
        double alpha = sinw0/(2.0f*Q);
    
        double A = 0.0;
        double a0inv = 0.0;
        double beta = 0.0;
    
        switch (filterType) {
            case BIQUAD_LOWPASS:
                a0inv = 1.0/(1.0 + alpha);
    
                b0 = (float)((1.0 - cosw0) * 0.5 * a0inv);
                b1 = (float)(2.0 * b0);
                b2 = (float)(b0);
                a1 = (float)(-2.0 * cosw0 * a0inv);
                a2 = (float)((1.0 - alpha) * a0inv);
                break;
    
            case BIQUAD_HIGHPASS:
                a0inv = 1.0/(1.0 + alpha);
    
                b0 = (float)((1.0 + cosw0) * 0.5 * a0inv);
                b1 = (float)(-2.0 * b0);
                b2 = (float)(b0);
                a1 = (float)(-2.0 * cosw0 * a0inv);
                a2 = (float)((1.0 - alpha) * a0inv);
                break;
    
            case BIQUAD_PEAK: 
                A = pow(10.0, gaindB/40.0);
                
                a0inv = 1/(1 + alpha/A);
    
                b0 = (float)((1.0 + alpha * A) * a0inv);
                b1 = (float)(-2.0 * cosw0 * a0inv);
                b2 = (float)((1.0 - alpha * A) * a0inv); 
                a1 = (float)(b1);
                a2 = (float)((1.0 - alpha / A) * a0inv);
                break;
    
            case BIQUAD_LOWSHELF: 
                A = pow(10.0, gaindB/40.0);
                beta = sqrt(A)/Q;

                a0inv = 1/((A+1) + (A-1)*cosw0 + beta*sinw0);
                
                b0 = (float)((A*((A+1) - (A-1)*cosw0 + beta*sinw0)) * a0inv);
                b1 = (float)((2*A*((A-1) - (A+1)*cosw0)) * a0inv);
                b2 = (float)((A*((A+1) - (A-1)*cosw0 - beta*sinw0)) * a0inv);
                a1 = (float)((-2*((A-1) + (A+1)*cosw0)) * a0inv);
                a2 = (float)(((A+1) + (A-1)*cosw0 - beta*sinw0) * a0inv);
                break;
    
            case BIQUAD_HIGHSHELF:
                A = pow(10.0, gaindB/40.0);
                beta = sqrt(A)/Q;
                a0inv = 1/((A+1) - (A-1)*cosw0 + beta*sinw0);
    
                b0 = (float)((A*((A+1) + (A-1)*cosw0 + beta*sinw0)) * a0inv);
                b1 = (float)((-2*A*((A-1) + (A+1)*cosw0)) * a0inv);
                b2 = (float)((A*((A+1) + (A-1)*cosw0 - beta*sinw0)) * a0inv);
                a1 = (float)((2*((A-1) - (A+1)*cosw0)) * a0inv);
                a2 = (float)(((A+1) - (A-1)*cosw0 - beta*sinw0) * a0inv);
                break;
    
            default:
                b0 = 1.0f;
                b1 = 0.0f;
                b2 = 0.0f;
                a1 = 0.0f;
                a2 = 0.0f;
                break;
        }
    }

    void process(float *bufferL, float *bufferR, size_t nSamples) {
        ZoneScoped;
    
        float lb0 = b0;
        float lb1 = b1;
        float lb2 = b2;
        float la1 = a1;
        float la2 = a2;


        if (bufferL) {
            float lw1L = w1L;
            float lw2L = w2L;

            for (size_t index = 0; index < nSamples; index++) {

                float input_sample = bufferL[index];        
                float w = input_sample - la1*lw1L - la2*lw2L;
                input_sample = lb0*w + lb1*lw1L + lb2*lw2L;
                
                lw2L = lw1L;
                lw1L = w;
                
                bufferL[index] = input_sample;    
            }
            
            w1L = lw1L;
            w2L = lw2L;
        }
        
        if (bufferR) {
            float lw1R = w1R;
            float lw2R = w2R;

            for (size_t index = 0; index < nSamples; index++) {

                float input_sample = bufferR[index];        
                float w = input_sample - la1*lw1R - la2*lw2R;
                input_sample = lb0*w + lb1*lw1R + lb2*lw2R;
                
                lw2R = lw1R;
                lw1R = w;
                
                bufferR[index] = input_sample;    
            }
            
            w1R = lw1R;
            w2R = lw2R;            
        }
    }


    float b0 = 1.0;
    float b1 = 0.0;
    float b2 = 0.0;
    float a1 = 0.0;
    float a2 = 0.0;

    float w1L = 0.0f;
    float w2L = 0.0f;
    float w1R = 0.0f;
    float w2R = 0.0f;
    FilterType filterType;

};


#endif // BIQUAD_H
