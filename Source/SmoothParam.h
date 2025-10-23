/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef PARAM_SMOOTHER_H
#define PARAM_SMOOTHER_H

#include <math.h>
#include <cstdint>
#include "FirstOrderFilter.h"

#define SMOOTH_PARAM_TIME 0.02f


struct SmoothParamLinear {

    ~SmoothParamLinear() { free(value_buffer); }

    void init(float initValue, u32 value_buffer_size) {
        target = initValue;
        currentValue = initValue;
        prevTarget = target;

        normValue = 0.0;
        isSmoothing = false;
        
        if (value_buffer_size != 0) {
            value_buffer = (float*)calloc(value_buffer_size, sizeof(float));
        }
    }

    void newTarget(float newTarget, float rampTimeMs, float samplerate) {
        ZoneScoped;
        prevTarget = target;
        target = newTarget;
        stepHeight = 1.0f / (rampTimeMs * 0.001f * samplerate);
        
        normValue = 0.0f;
        isSmoothing = true;
    }

    inline float nextValue() {
        ZoneScoped;
        if (!isSmoothing) { return target; }

        normValue += stepHeight;
        if (normValue >= 1.0) {
            isSmoothing = false;
            normValue = 1.0;
            currentValue = target;
            return currentValue;
        }

        currentValue = normValue * (target - prevTarget) + prevTarget;
        return currentValue;
    }

    void generateBufferOfValues(u32 nsamples) {
        ZoneScoped;
        assert(value_buffer && "SmoothParamLinear : value_buffer non allocated");        
        
        for (u32 index = 0; index < nsamples; index++) {
            if (!isSmoothing) { 
                value_buffer[index] = target;
                continue; 
            }
    
            normValue += stepHeight;
            if (normValue >= 1.0) {
                isSmoothing = false;
                normValue = 1.0;
                currentValue = target;
                value_buffer[index] = currentValue;
                continue;
            }
    
            currentValue = normValue * (target - prevTarget) + prevTarget;
            value_buffer[index] = currentValue;
        }
    }

    
    // inline void applySmoothGainDeciBels(float *bufferL, float *bufferR, u32 nSamples) {
    //     ZoneScoped;
    //     if (bufferR) {
    //         for (size_t i = 0; i < nSamples; i++) {
    //             float gainValue = (float)dbtoa(nextValue());
    //             bufferL[i] *= gainValue;
    //             bufferR[i] *= gainValue;
    //         }
    //         return;
    //     }

    //     for (size_t i = 0; i < nSamples; i++) {
    //         bufferL[i] *= (float)dbtoa(nextValue());
    //     }
    // }

    inline void applySmoothGainLinear(float *bufferL, float *bufferR, u32 nSamples) {
        ZoneScoped;
        
        generateBufferOfValues(nSamples);

        for (size_t i = 0; i < nSamples; i++) {
            bufferL[i] *= value_buffer[i];
        }

        if (bufferR) {
            for (size_t i = 0; i < nSamples; i++) {
                float gainValue = value_buffer[i];
                bufferR[i] *= gainValue;
            }
        }
    }


    float target = 0.0;
    float prevTarget = 0.0;
    float stepHeight = 0.0;
    float normValue = 0.0;
    float currentValue = 0.0;
    float *value_buffer = nullptr;
    bool isSmoothing;
};

// supprimer et remplacer le gain de la gate par un onepole
// ca sera plus simple
struct SmoothParamIIR {

    void init(float initValue) {
        currentValue = initValue;
        target = initValue;
        y1 = initValue;

        b0 = 1.0;
        a1 = 0.0;
    }

    void newTarget(float newTarget, float tauMs, float samplerate) {
        ZoneScoped;
        b0 = std::sin((float)M_PI / (samplerate * tauMs * 0.001f));
        a1 = b0 - 1.0f;
        target = newTarget;
    }

    float nextValue() {
        ZoneScoped;
        currentValue = target * b0 - y1 * a1;
        y1 = currentValue;

        return currentValue;
    }

    float currentValue;
    float target;
    float b0, a1;
    float y1;

};

#endif // PARAM_SMOOTHER_H
