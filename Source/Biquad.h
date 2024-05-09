/*
==============================================================================
    Author:  Benjamin Quiedeville
==============================================================================
*/

#ifndef BIQUAD_H
#define BIQUAD_H

#include "common.h"

enum FilterType {
    BIQUAD_LOWPASS = 0,
    BIQUAD_HIGHPASS,
    BIQUAD_PEAK,
    BIQUAD_LOWSHELF,
    BIQUAD_HIGHSHELF,
    BIQUAD_NFILTERTYPES,
};

//@TODO refactor to be thread safe
struct Biquad {

    Biquad(FilterType type) { filterType = (u8)type; }

    void prepareToPlay() {

        b0 = 1.0;
        b1 = 0.0;
        b2 = 0.0;
        a1 = 0.0;
        a2 = 0.0;
        reset();
    }

    void reset() {
        w1 = 0.0f;
        w2 = 0.0f;
    }

    void setCoefficients(double frequency, double Q, double gaindB, double samplerate) {
    
        double w0 = juce::MathConstants<double>::twoPi / samplerate * frequency;
        double cosw0 = cos(w0);
        double sinw0 = sin(w0);
    
        double alpha = sinw0/(2.0f*Q);
    
        double A = 0.0;
        double a0inv = 0.0;
        double beta = 0.0;
    
        switch (filterType) {
            case BIQUAD_LOWPASS:
                a0inv = 1.0/(1.0 + alpha);
    
                b0 = (sample_t)((1.0 - cosw0) * 0.5 * a0inv);
                b1 = (sample_t)(2.0 * b0);
                b2 = (sample_t)(b0);
                a1 = (sample_t)(-2.0 * cosw0 * a0inv);
                a2 = (sample_t)((1.0 - alpha) * a0inv);
                break;
    
            case BIQUAD_HIGHPASS:
                a0inv = 1.0/(1.0 + alpha);
    
                b0 = (sample_t)((1.0 + cosw0) * 0.5 * a0inv);
                b1 = (sample_t)(-2.0 * b0);
                b2 = (sample_t)(b0);
                a1 = (sample_t)(-2.0 * cosw0 * a0inv);
                a2 = (sample_t)((1.0 - alpha) * a0inv);
                break;
    
            case BIQUAD_PEAK: 
                A = pow(10.0, gaindB/40.0);
                
                a0inv = 1/(1 + alpha/A);
    
                b0 = (sample_t)((1.0 + alpha * A) * a0inv);
                b1 = (sample_t)(-2.0 * cosw0 * a0inv);
                b2 = (sample_t)((1.0 - alpha * A) * a0inv); 
                a1 = (sample_t)(b1);
                a2 = (sample_t)((1.0 - alpha / A) * a0inv);
                break;
    
            case BIQUAD_LOWSHELF: 
                A = pow(10.0, gaindB/40.0);
                beta = sqrt(A)/Q;

                a0inv = 1/((A+1) + (A-1)*cosw0 + beta*sinw0);
                
                b0 = (sample_t)((A*((A+1) - (A-1)*cosw0 + beta*sinw0)) * a0inv);
                b1 = (sample_t)((2*A*((A-1) - (A+1)*cosw0)) * a0inv);
                b2 = (sample_t)((A*((A+1) - (A-1)*cosw0 - beta*sinw0)) * a0inv);
                a1 = (sample_t)((-2*((A-1) + (A+1)*cosw0)) * a0inv);
                a2 = (sample_t)(((A+1) + (A-1)*cosw0 - beta*sinw0) * a0inv);
                break;
    
            case BIQUAD_HIGHSHELF:
                A = pow(10.0, gaindB/40.0);
                beta = sqrt(A)/Q;
                a0inv = 1/((A+1) - (A-1)*cosw0 + beta*sinw0);
    
                b0 = (sample_t)((A*((A+1) + (A-1)*cosw0 + beta*sinw0)) * a0inv);
                b1 = (sample_t)((-2*A*((A-1) + (A+1)*cosw0)) * a0inv);
                b2 = (sample_t)((A*((A+1) + (A-1)*cosw0 - beta*sinw0)) * a0inv);
                a1 = (sample_t)((2*((A-1) - (A+1)*cosw0)) * a0inv);
                a2 = (sample_t)(((A+1) - (A-1)*cosw0 - beta*sinw0) * a0inv);
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


    void processBuffer(sample_t *signal, size_t nSamples) {

        for (size_t i = 0; i < nSamples; i++) {
            signal[i] = process(signal[i]);
        }
    }


    sample_t process(sample_t sample) {

        sample_t w = sample - a1*w1 - a2*w2;
        sample_t outputSample = b0*w + b1*w1 + b2*w2;
        
        w2 = w1;
        w1 = w;
        
        return outputSample;
    }


    sample_t b0 = 1.0;
    sample_t b1 = 0.0;
    sample_t b2 = 0.0;
    sample_t a1 = 0.0;
    sample_t a2 = 0.0;

    sample_t w1 = 0.0f;
    sample_t w2 = 0.0f;
    u8 filterType;

};


#endif // BIQUAD_H
