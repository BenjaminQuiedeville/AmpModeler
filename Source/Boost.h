/*
  ==============================================================================

    preBoost.h
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef PREBOOST_H
#define PREBOOST_H

#include "common.h"
#include "Biquad.h"
#include "OnepoleFilter.h"
#include "SmoothParam.h"

#define BOOST_BITE_Q     0.4
#define BOOST_BITE_FREQ  1700.0

struct Boost {

    Boost() {
        biteFilter = new Biquad(FilterType::BIQUAD_PEAK);
    }

    ~Boost() {
        delete biteFilter;
    }

    void prepareToPlay() {
        tightFilter.prepareToPlay();
        biteFilter->prepareToPlay();
    }

    inline void process(float *buffer, size_t nSamples) {

        for (size_t i = 0; i<nSamples; i++) {
            sample_t sample = buffer[i];
            sample = tightFilter.processHighPass(sample);
            sample = biteFilter->process(sample);
            buffer[i] = sample;
        }
    }

    OnepoleFilter tightFilter;
    Biquad *biteFilter;
};

#endif // PREBOOST_H