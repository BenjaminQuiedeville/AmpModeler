/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef PREBOOST_H
#define PREBOOST_H

#include "common.h"
#include "Biquad.h"
#include "OnepoleFilter.h"
#include "SmoothParam.h"

#define BOOST_BITE_Q     0.23

struct Boost {

    void prepareToPlay() {
        tightFilter.prepareToPlay();
        biteFilter.prepareToPlay();
    }

    inline void process(float *buffer, size_t nSamples) {

        for (size_t i = 0; i<nSamples; i++) {
            sample_t sample = buffer[i];
            sample = tightFilter.processHighPass(sample);
            sample = biteFilter.process(sample);
            buffer[i] = sample;
        }
    }

    OnepoleFilter tightFilter;
    Biquad biteFilter {BIQUAD_PEAK};
};

#endif // PREBOOST_H