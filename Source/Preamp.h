/*
  ==============================================================================

    PreampDistorsion.h
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#ifndef PREAMP_H
#define PREAMP_H

#include "common.h"
#include "OnepoleFilter.h"
#include "Biquad.h"
#include "SmoothParam.h"

#define STAGE_GAIN                     (sample_t)DB_TO_GAIN(30.0)
#define OUTPUT_ATTENUATION             (sample_t)DB_TO_GAIN(-20.0)
#define PREAMP_UP_SAMPLE_FACTOR        4

/*
    changer l'algo de distorsion pour une courbe asymétrique haute headroom
*/

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
    void process(float *buffer, size_t nSamples);

    SmoothParamLinear preGain;
    SmoothParamLinear postGain;
    double samplerate = 0.0;

    OnepoleFilter inputFilter;

    OnepoleFilter couplingFilter1;
    OnepoleFilter couplingFilter2;
    OnepoleFilter couplingFilter3;

    OnepoleFilter stageOutputFilter1;
    OnepoleFilter stageOutputFilter2;
    OnepoleFilter stageOutputFilter3;

    Biquad tubeBypassFilter1 {BIQUAD_HIGHSHELF};
    Biquad tubeBypassFilter2 {BIQUAD_HIGHSHELF};

    OverSampler *overSampler;
    sample_t *upSampledBlock = nullptr;

    sample_t headroom = 15.0f;

};

#endif // PREAMP_H