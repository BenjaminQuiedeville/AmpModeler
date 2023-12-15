/*
  ==============================================================================

    preBoost.h
    Created: 15 May 2023 12:24:13am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef PREBOOST_H
#define PREBOOST_H

#include "JuceHeader.h"
#include "common.h"
#include "Biquad.h"
#include "OnepoleFilter.h"
#include "SmoothParam.h"

#define BOOST_BITE_Q     0.5
#define BOOST_BITE_FREQ  1700.0

struct Boost {

    Boost() {
        tightFilter = new OnepoleFilter();
        biteFilter = new Biquad(FilterType::BIQUAD_PEAK);
        level = new SmoothParam();
    }

    ~Boost() {
        delete tightFilter;
        delete biteFilter;
        delete level;
    }

    void prepareToPlay(double _samplerate) {
        samplerate = _samplerate;
        biteFilter->prepareToPlay();
        level->init(_samplerate, 0.02, 0.0, CurveType::SMOOTH_PARAM_LIN);
    }

    inline void process(float *buffer, size_t nSamples) {

        for (size_t i = 0; i<nSamples; i++) {
            sample_t sample = buffer[i];
            sample = tightFilter->processHighPass(sample);
            sample = biteFilter->process(sample);
            buffer[i] = sample * (sample_t)DB_TO_GAIN(level->nextValue());
        }

    }

    OnepoleFilter *tightFilter;
    Biquad *biteFilter;
    SmoothParam *level;
    double samplerate;

};

#endif // PREBOOST_H