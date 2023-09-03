/*
  ==============================================================================

    ThreeBandEQ.h
    Created: 23 Apr 2023 9:08:03pm
    Author:  Benjamin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Biquad.h"


struct ThreeBandEQ {

    ThreeBandEQ();
    ~ThreeBandEQ();

    void prepareToPlay(double _samplerate);
    void process(float *input, size_t nSamples);
    void updateGains(double bassGain,
                     double midGain,
                     double trebbleGain);


    Biquad *bassFilters;
    Biquad *midFilters;
    Biquad *trebbleFilters;

    const double bassFreq = 120.0;
    const double midFreq = 750.0;
    const double trebbleFreq = 3500.0;
    
    const double bassQ = 0.5;
    const double midQ = 0.5;
    const double trebbleQ = 0.5;
};
