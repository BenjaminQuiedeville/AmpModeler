/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef ONEPOLE_H
#define ONEPOLE_H

#include "common.h"


struct FirstOrderFilter {

    void reset() {
        b0 = 1.0;
        b1 = 0.0;
        a1 = 0.0;
        x1L = 0.0f;
        x1R = 0.0f;
        y1L = 0.0f;
        y1R = 0.0f;
        outGain = 1.0f;
    }

    void makeLowpass(float frequency, float samplerate) {
        ZoneScoped;
        float K = (float)tan(M_PI * frequency/samplerate);
        
        float inv_a0 = 1.0f/(K+1);
        
        b0 = K * inv_a0;
        b1 = b0;
        a1 = -(K-1.0f) * inv_a0;
        outGain = 1.0f;
    }
    
    void makeHighpass(float frequency, float samplerate) {
        ZoneScoped;
        float K = (float)tan(M_PI * frequency/samplerate);
        
        float inv_a0 = 1.0f/(K+1);
        
        b0 = inv_a0;
        b1 = -b0;
        a1 = -(K-1.0f) * inv_a0;
        outGain = 1.0f;
    }


    void makeLowShelf(float freq, float gain_db, float samplerate) {
        ZoneScoped;

        outGain = (float)dbtoa(gain_db);
        float gainLinear = (float)dbtoa(-gain_db);
        
        double freqRadian = freq / samplerate * M_PI;

        double eta = (gainLinear + 1.0)/(gainLinear - 1.0);
        double rho = sin(M_PI * freqRadian * 0.5 - M_PI_4) 
                    / sin(M_PI * freqRadian * 0.5 + M_PI_4);

        double etaSign = eta > 0.0 ? 1.0 : -1.0;
        double alpha1 = gainLinear == 1.0 ? 0.0 : eta - etaSign*sqrt(eta*eta - 1.0);

        double beta0 = ((1 + gainLinear) + (1 - gainLinear) * alpha1) * 0.5;
        double beta1 = ((1 - gainLinear) + (1 + gainLinear) * alpha1) * 0.5;

        b0 = (float)((beta0 + rho * beta1)/(1 + rho * alpha1));
        b1 = (float)((beta1 + rho * beta0)/(1 + rho * alpha1));
        a1 = -(float)((rho + alpha1)/(1 + rho * alpha1));
    }
    
    
    void makeHighShelf(float freq, float gain_db, double samplerate) {
        ZoneScoped;
        
        outGain = 1.0f;
        float gainLinear = (float)dbtoa(gain_db);

        double freqRadian = freq / samplerate * M_PI;

        double eta = (gainLinear + 1.0)/(gainLinear - 1.0);
        double rho = sin(M_PI * freqRadian * 0.5 - M_PI_4) 
                    / sin(M_PI * freqRadian * 0.5 + M_PI_4);

        double etaSign = eta > 0.0 ? 1.0 : -1.0;
        double alpha1 = gainLinear == 1.0 ? 0.0 : eta - etaSign*sqrt(eta*eta - 1.0);

        double beta0 = ((1 + gainLinear) + (1 - gainLinear) * alpha1) * 0.5;
        double beta1 = ((1 - gainLinear) + (1 + gainLinear) * alpha1) * 0.5;

        b0 = (float)((beta0 + rho * beta1)/(1 + rho * alpha1));
        b1 = (float)((beta1 + rho * beta0)/(1 + rho * alpha1));
        a1 = -(float)((rho + alpha1)/(1 + rho * alpha1));
    }

    void process(float *bufferL, float *bufferR, u64 nSamples) {
        ZoneScoped;
        
        if (bufferL) {            
            for (u64 index = 0; index < nSamples; index++) {
                float inSample = bufferL[index];
                float outSample = inSample*b0 + x1L*b1 + y1L*a1;
                
                x1L = inSample;
                y1L = outSample;
                bufferL[index] = outSample * outGain;
            }
        }
            
        if (bufferR) {            
            for (u64 index = 0; index < nSamples; index++) {
                float inSample = bufferR[index];
                float outSample = inSample*b0 + x1R*b1 + y1R*a1;
                
                x1R = inSample;
                y1R = outSample;
                bufferR[index] = outSample * outGain;
            }
        }
    }

    
    float b0 = 1.0;
    float b1 = 0.0;
    float a1 = 0.0;
    float x1L = 0.0f;
    float x1R = 0.0f;
    float y1L = 0.0f;
    float y1R = 0.0f;
    float outGain = 1.0f;
};

#endif // ONEPOLE_H
