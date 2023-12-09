/*
  ==============================================================================

    preBoost.h
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef PREBOOST_H
#define PREBOOST_H

#include <JuceHeader.h>
#include "common.h"
#include "Biquad.h"
#include "OnepoleFilter.h"

struct Boost {

    Boost();
    ~Boost();

    void prepareToPlay(double _samplerate);
    void updateTight(const float newFrequency);
    void updateBite(const float newGain);
    inline void process(float *buffer, size_t nSamples) {

        tightFilter->processBufferHighpass(buffer, nSamples);
        biteFilter->process(buffer, nSamples);

    }

    float tightFrequency;
    const float biteFrequency = 1700.0f;

    // float tightQ = 0.7f;
    float biteQ = 0.5f;

    // float biteGain;

    OnepoleFilter *tightFilter;
    Biquad *biteFilter;

};

#endif // PREBOOST_H