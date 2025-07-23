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
        x1L = 0.0f;
        x2L = 0.0f;
        y1L = 0.0f;
        y2L = 0.0f;
        
        x1R = 0.0f;
        x2R = 0.0f;
        y1R = 0.0f;
        y2R = 0.0f;
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
                a1 = -(float)(-2.0 * cosw0 * a0inv);
                a2 = -(float)((1.0 - alpha) * a0inv);
                break;
    
            case BIQUAD_HIGHPASS:
                a0inv = 1.0/(1.0 + alpha);
    
                b0 = (float)((1.0 + cosw0) * 0.5 * a0inv);
                b1 = (float)(-2.0 * b0);
                b2 = (float)(b0);
                a1 = -(float)(-2.0 * cosw0 * a0inv);
                a2 = -(float)((1.0 - alpha) * a0inv);
                break;
    
            case BIQUAD_PEAK: 
                A = pow(10.0, gaindB/40.0);
                
                a0inv = 1/(1 + alpha/A);
    
                b0 = (float)((1.0 + alpha * A) * a0inv);
                b1 = (float)(-2.0 * cosw0 * a0inv);
                b2 = (float)((1.0 - alpha * A) * a0inv); 
                a1 = -(float)(b1);
                a2 = -(float)((1.0 - alpha / A) * a0inv);
                break;
    
            case BIQUAD_LOWSHELF: 
                A = pow(10.0, gaindB/40.0);
                beta = sqrt(A)/Q;

                a0inv = 1/((A+1) + (A-1)*cosw0 + beta*sinw0);
                
                b0 = (float)((A*((A+1) - (A-1)*cosw0 + beta*sinw0)) * a0inv);
                b1 = (float)((2*A*((A-1) - (A+1)*cosw0)) * a0inv);
                b2 = (float)((A*((A+1) - (A-1)*cosw0 - beta*sinw0)) * a0inv);
                a1 = -(float)((-2*((A-1) + (A+1)*cosw0)) * a0inv);
                a2 = -(float)(((A+1) + (A-1)*cosw0 - beta*sinw0) * a0inv);
                break;
    
            case BIQUAD_HIGHSHELF:
                A = pow(10.0, gaindB/40.0);
                beta = sqrt(A)/Q;
                a0inv = 1/((A+1) - (A-1)*cosw0 + beta*sinw0);
    
                b0 = (float)((A*((A+1) + (A-1)*cosw0 + beta*sinw0)) * a0inv);
                b1 = (float)((-2*A*((A-1) + (A+1)*cosw0)) * a0inv);
                b2 = (float)((A*((A+1) + (A-1)*cosw0 - beta*sinw0)) * a0inv);
                a1 = -(float)((2*((A-1) - (A+1)*cosw0)) * a0inv);
                a2 = -(float)(((A+1) - (A-1)*cosw0 - beta*sinw0) * a0inv);
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

        
        float x1 = x1L;
        float x2 = x2L;
        float y1 = y1L;
        float y2 = y2L;

        for (size_t index = 0; index < nSamples; index++) {

            float input_sample = bufferL[index];
            float result = input_sample * lb0 
                         + x1 * lb1
                         + x2 * lb2
                         + y1 * la1
                         + y2 * la2;
            
            x2 = x1;
            x1 = input_sample;
            y2 = y1;
            y1 = result;
            
            bufferL[index] = result;    
        }
        
        x1L = x1;
        x2L = x2;
        y1L = y1;    
        y2L = y2;
        
        if (bufferR) {
            x1 = x1R;
            x2 = x2R;
            y1 = y1R;
            y2 = y2R;

            for (size_t index = 0; index < nSamples; index++) {

                float input_sample = bufferR[index];
                float result = input_sample * lb0 
                             + x1 * lb1
                             + x2 * lb2
                             + y1 * la1
                             + y2 * la2;
                
                x2 = x1;
                x1 = input_sample;
                y2 = y1;
                y1 = result;
                
                bufferR[index] = result;    
            }
            
            x1R = x1;
            x2R = x2;
            y1R = y1;    
            y2R = y2;
        }
    }


    float b0 = 1.0f;
    float b1 = 0.0f;
    float b2 = 0.0f;
    float a1 = 0.0f;
    float a2 = 0.0f;

    float x1L = 0.0f;
    float x2L = 0.0f;
    float y1L = 0.0f;
    float y2L = 0.0f;
    
    float x1R = 0.0f;
    float x2R = 0.0f;
    float y1R = 0.0f;
    float y2R = 0.0f;
    
    FilterType filterType;
};


#endif // BIQUAD_H
