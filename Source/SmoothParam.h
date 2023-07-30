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

    SmoothParam() {}
    ~SmoothParam() {}

    void init(double _samplerate, double _rampTime, double initValue) {
        samplerate = _samplerate;
        rampTimeMs = _rampTime;

        currentValue = initValue;
    }

    void newTarget(double _target) {
        target = _target;
        stepHeight = abs(target - currentValue) / (rampTimeMs * 0.001 * samplerate);
    }
    
    double nextValue() {
        currentValue = currentValue + stepHeight > target
                     ? target
                     : currentValue + stepHeight;
        return currentValue; 
    }

    double samplerate;
    double stepHeight;
    double currentValue;
    double target;
    double rampTimeMs;
};

#endif // _PARAM_SMOOTHER