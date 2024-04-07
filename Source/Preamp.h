/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef PREAMP_H
#define PREAMP_H

#include "common.h"
#include "OnepoleFilter.h"
#include "Biquad.h"
#include "SmoothParam.h"

#define PREAMP_UP_SAMPLE_FACTOR        4



struct OverSampler {

    void prepareToPlay(double _samplerate);

    void upSample(sample_t *source, sample_t *upSampled, size_t sourceSize, size_t upSampledSize);
    void downSample(sample_t *upSampled, sample_t *dest, size_t upSampledSize, size_t destSize);

    Biquad upSampleFilter1 {BIQUAD_LOWPASS};
    Biquad upSampleFilter2 {BIQUAD_LOWPASS};
    
    Biquad downSampleFilter1 {BIQUAD_LOWPASS};
    Biquad downSampleFilter2 {BIQUAD_LOWPASS};
};


struct PreampDistorsion {
    PreampDistorsion();
    ~PreampDistorsion();
    
    void prepareToPlay(double samplerate, int blockSize);
    sample_t processGainStages(sample_t sample);
    void process(sample_t *buffer, size_t nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;
    double samplerate = 0.0;

    OnepoleFilter inputFilter;
    
    OnepoleFilter couplingFilter1;
    OnepoleFilter couplingFilter2;
    OnepoleFilter couplingFilter3;
    OnepoleFilter couplingFilter4;

    OnepoleFilter stageOutputFilter1;
    OnepoleFilter stageOutputFilter2;
    OnepoleFilter stageOutputFilter3;
    OnepoleFilter stageOutputFilter4;

    Biquad cathodeBypassFilter1 {BIQUAD_LOWSHELF};
    Biquad cathodeBypassFilter2 {BIQUAD_LOWSHELF};
    Biquad cathodeBypassFilter3 {BIQUAD_LOWSHELF};
    Biquad cathodeBypassFilter4 {BIQUAD_LOWSHELF};

    OverSampler *overSampler;
    sample_t *upSampledBlock = nullptr;

    sample_t headroom = 20.0f;
    u8 channel = 2;

};

#endif // PREAMP_H