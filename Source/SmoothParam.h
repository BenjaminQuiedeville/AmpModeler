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

struct SmoothParam {

    enum CurveType {
        LIN,
        TANH,
        ALGEBRAIC,
    };

    SmoothParam() {}
    ~SmoothParam() {}

    void init(double _samplerate, double _rampTimeMs, double initValue, uint8_t _curveType) {
        samplerate = _samplerate;
        rampTimeMs = _rampTimeMs;

        currentValue = initValue;
        curveType = _curveType;
    }

    void newTarget(double _target) {
        target = _target;
        stepHeight = abs(target - currentValue) / (rampTimeMs * 0.001 * samplerate);
    }
    
    double nextValue() {

        switch (curveType) {
        case LIN:
                currentValue = currentValue + stepHeight > target
                             ? target
                             : currentValue + stepHeight;
                break;

        case TANH:      break;
        case ALGEBRAIC: break;

        }
        return currentValue;
    }

    double samplerate;
    double stepHeight;
    double currentValue;
    double target;
    double rampTimeMs;

    uint8_t curveType;
};

#endif // _PARAM_SMOOTHER
