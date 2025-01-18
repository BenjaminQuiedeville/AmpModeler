#ifndef FIRST_ORDER_SHELF_H
#define FIRST_ORDER_SHELF_H

#include "common.h"

enum ShelfType : u8 { lowshelf, highshelf };

struct FirstOrderShelfFilter {

    FirstOrderShelfFilter(ShelfType type_) { filterType = type_; }

    void prepareToPlay() {
        b0 = 1.0f;
        b1 = 0.0f;
        a1 = 0.0f;
        x1L = 0.0f;
        y1L = 0.0f;
        x1R = 0.0f;
        y1R = 0.0f;
    }
    
    void setCoefficients(float freq, float gain_db, double samplerate) {

        float gainLinear = 1.0f;

        if (filterType == ShelfType::lowshelf) {
            outGain = (float)dbtoa(gain_db);
            gainLinear = (float)dbtoa(-gain_db);
        
        } else if (filterType == ShelfType::highshelf) {
            outGain = 1.0f;
            gainLinear = (float)dbtoa(gain_db);

        } else {
            assert(false && "wrong type of filter");
        } 

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
        a1 = (float)((rho + alpha1)/(1 + rho * alpha1));
    }

    inline void process(float *bufferL, float *bufferR, u32 nSamples) {
        ZoneScoped;
        if (bufferL) {
            for (u32 i = 0; i < nSamples; i++) {
                float outSample = b0 * bufferL[i] 
                                 + b1 * x1L 
                                 - a1 * y1L;
    
                x1L = bufferL[i];
                y1L = outSample;
                bufferL[i] = outSample * outGain; 
    
            }
        }
        
        if (bufferR) {
            for (u32 i = 0; i < nSamples; i++) {
                float outSample = b0 * bufferR[i] 
                          + b1 * x1R 
                          - a1 * y1R;
    
                x1R = bufferR[i];
                y1R = outSample;
                bufferR[i] = outSample * outGain; 
            }    
        }
    }

    float b0 = 1.0f;
    float b1 = 0.0f;
    float a1 = 0.0f;
    float x1L = 0.0f;
    float y1L = 0.0f;
    float x1R = 0.0f;
    float y1R = 0.0f;
    float outGain = 1.0f; 
    ShelfType filterType;
};

#endif // FIRST_ORDER_SHELF_H
