/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef PARAM_SMOOTHER_H
#define PARAM_SMOOTHER_H

#include <math.h>
#include <cstdint>
#include "OnepoleFilter.h"

#define SMOOTH_PARAM_TIME 0.02


struct SmoothParamLinear {

    void init(double initValue) {
        target = initValue;
        currentValue = initValue;
        prevTarget = target;

        normValue = 0.0;
        isSmoothing = false;
    }

    void newTarget(double newTarget, double rampTimeMs, double samplerate) {
        prevTarget = target;
        target = newTarget;
        stepHeight = 1.0 / (rampTimeMs * 0.001 * samplerate);

        isSmoothing = true;
    }

    inline double nextValue() {

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

    inline void applySmoothGainDeciBels(float *bufferL, float *bufferR, u32 nSamples) {
        ZoneScoped;
        if (bufferR) {
            for (size_t i = 0; i < nSamples; i++) {
                float gainValue = (float)dbtoa(nextValue());
                bufferL[i] *= gainValue;
                bufferR[i] *= gainValue;
            }
            return;
        }

        for (size_t i = 0; i < nSamples; i++) {
            bufferL[i] *= (float)dbtoa(nextValue());
        }
    }

    inline void applySmoothGainLinear(float *bufferL, float *bufferR, u32 nSamples) {
        ZoneScoped;
        if (bufferR) {
            for (size_t i = 0; i < nSamples; i++) {
                float gainValue = (float)nextValue();
                bufferL[i] *= gainValue;
                bufferR[i] *= gainValue;
            }
            return;
        }

        for (size_t i = 0; i < nSamples; i++) {
            bufferL[i] *= (float)nextValue();
        }
    }


    double target = 0.0;
    double prevTarget = 0.0;
    double stepHeight = 0.0;
    double normValue = 0.0;
    double currentValue = 0.0;

    bool isSmoothing;
};

// supprimer et remplacer le gain de la gate par un onepole
// ca sera plus simple
struct SmoothParamIIR {

    void init(double initValue) {
        currentValue = initValue;
        target = initValue;
        y1 = initValue;

        b0 = 1.0;
        a1 = 0.0;
    }

    void newTarget(double newTarget, double tauMs, double samplerate) {
        b0 = std::sin(M_PI / (samplerate * tauMs * 0.001));
        a1 = b0 - 1.0;
        target = newTarget;
    }

    double nextValue() {
        currentValue = target * b0 - y1 * a1;
        y1 = currentValue;

        return currentValue;
    }

    double currentValue;
    double target;
    double b0, a1;
    double y1;

};

#endif // PARAM_SMOOTHER_H
