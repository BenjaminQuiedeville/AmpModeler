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

    double nextValue() {

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

    double target;
    double prevTarget;
    double stepHeight;
    double normValue;
    double currentValue;

    bool isSmoothing;
};  


struct SmoothParamIIR {

    void init(double initValue) {
        currentValue = initValue;
        target = initValue;
        y1 = initValue;

        b0 = 1.0;
        a1 = 0.0;
    }
    
    void newTarget(double newTarget, double tauMs, double samplerate) {
        b0 = std::sin(juce::MathConstants<double>::pi / (samplerate * tauMs * 0.001));
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
