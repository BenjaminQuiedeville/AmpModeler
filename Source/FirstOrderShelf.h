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
        ZoneScoped;
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

    void process(float *bufferL, float *bufferR, u32 nSamples) {
        ZoneScoped;
        
        float lb0 = this->b0;
        float lb1 = this->b1;
        float la1 = this->a1;
        float loutGain = this->outGain;
    
    
        float lx1L = this->x1L;
        float ly1L = this->y1L;


        for (u32 index = 0; index < nSamples; index++) {
            float input_sample = bufferL[index];
            float outSample = lb0 * input_sample
                            + lb1 * lx1L 
                            - la1 * ly1L;

            lx1L = input_sample;
            ly1L = outSample;
            bufferL[index] = outSample * loutGain; 
        }
        
        this->x1L = lx1L;
        this->y1L = ly1L;
        
        if (bufferR) {
            float lx1R = x1R;
            float ly1R = y1R;

            for (u32 index = 0; index < nSamples; index++) {
                float input_sample = bufferR[index];
                float outSample = lb0 * input_sample
                                 + lb1 * lx1R 
                                 - la1 * ly1R;
    
                lx1R = input_sample;
                ly1R = outSample;
                bufferR[index] = outSample * loutGain; 
            }
            
            x1R = lx1R;
            y1R = ly1R;
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
