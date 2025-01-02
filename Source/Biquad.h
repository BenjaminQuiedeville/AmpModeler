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

        if (bufferL) {
            for (size_t index = 0; index < nSamples; index++) {
        
                float w = bufferL[index] - a1*w1L - a2*w2L;
                bufferL[index] = b0*w + b1*w1L + b2*w2L;
                
                w2L = w1L;
                w1L = w;        
            }
        }
        
        if (bufferR) {
            for (size_t index = 0; index < nSamples; index++) {
    
                float w = bufferR[index] - a1*w1R - a2*w2R;
                bufferR[index] = b0*w + b1*w1R + b2*w2R;
                
                w2R = w1R;
                w1R = w;        
            }
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
