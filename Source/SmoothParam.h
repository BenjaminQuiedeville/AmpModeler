/*
  ==============================================================================

    SmoothParam.h
    Created: 30 Jul 2023 1:41:07pm
    Author:  benjamin

  ==============================================================================
*/

#ifndef _PARAM_SMOOTHER
#define _PARAM_SMOOTHER

#include <math.h>
#include <cassert>

enum CurveType {
    SMOOTH_PARAM_LIN,
    SMOOTH_PARAM_TANH,
    SMOOTH_PARAM_ALGEBRAIC,
};

struct SmoothParam {


    SmoothParam() {}
    ~SmoothParam() {}

    void init(double _samplerate, double _rampTimeMs, double initValue, uint8_t _curveType) {
        samplerate = _samplerate;
        rampTimeMs = _rampTimeMs;

        currentValue = initValue;
        curveType = _curveType;
        isSmoothing = false;
    }

    void newTarget(double _target) {
        prevTarget = currentTarget;
        currentTarget = _target;
        stepHeight = 1.0 / (rampTimeMs * 0.001 * samplerate);

        currentNormValue = 0.0;
        isSmoothing = true;
    }
    
    double nextValue() {

        if (!isSmoothing) { return currentTarget; }

        switch (curveType) {
            case SMOOTH_PARAM_LIN:
                currentValue = currentNormValue * (currentTarget - prevTarget) + prevTarget;

                break;

            case SMOOTH_PARAM_TANH:
            case SMOOTH_PARAM_ALGEBRAIC:
                double x = 2.0 * (currentNormValue * 2.0 - 1.0);
                double algebraic = x / (1.0 + abs(x));
                double normalized = (algebraic * 1.5 + 1.0) * 0.5;

                currentValue = normalized * (currentTarget - prevTarget) + prevTarget;
                break;
        }

        currentNormValue += stepHeight;
        if (currentNormValue >= 1.0) {
            isSmoothing = false;
            currentNormValue = 1.0;
        }

        return currentValue;
    }

    double samplerate;
    double stepHeight;
    double currentNormValue;
    double currentValue;
    double currentTarget;
    double prevTarget;
    double rampTimeMs;

    uint8_t curveType;

    bool isSmoothing;
};

#endif // _PARAM_SMOOTHER
