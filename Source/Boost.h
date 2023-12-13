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
#include "SmoothParam.h"

#define BOOST_BITE_Q     0.5
#define BOOST_BITE_FREQ  1700.0

struct Boost {

    Boost();
    ~Boost();

    void prepareToPlay(double _samplerate);
    void updateTight(const float newFrequency);
    void updateBite(const float newGain);
    inline void process(float *buffer, size_t nSamples) {

        for (size_t i = 0; i<nSamples; i++) {
            sample_t sample = buffer[i];
            sample = tightFilter->processHighPass(sample);
            sample = biteFilter->process(sample);
            buffer[i] = sample * DB_TO_GAIN(level->nextValue());
        }

    }


    OnepoleFilter *tightFilter;
    Biquad *biteFilter;
    SmoothParam *level;

};

#endif // PREBOOST_H