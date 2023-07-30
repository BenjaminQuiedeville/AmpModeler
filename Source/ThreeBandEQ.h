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

    void prepareToPlay(double _samplerate²);
    void process(AudioBlock& audioBlock);
    void updateGains(const float bassGain, 
                     const float midGain, 
                     const float trebbleGain);


    Biquad *bassFilters;
    Biquad *midFilters;
    Biquad *trebbleFilters;

    const float bassFreq = 120.0f;
    const float midFreq = 750.0f;
    const float trebbleFreq = 3500.0f;
    
    const float bassQ = 0.5;
    const float midQ = 0.5;
    const float trebbleQ = 0.5;
};