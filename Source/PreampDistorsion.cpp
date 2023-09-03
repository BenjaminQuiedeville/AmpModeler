/*
  ==============================================================================

    PreampDistorsion.cpp
    Created: 2 May 2023 1:19:14am
    Author:  Benjamin

  ==============================================================================
*/

#include "PreampDistorsion.h"

PreampDistorsion::PreampDistorsion() {
    
    overSampler = new juce::dsp::Oversampling<sample_t>(
        1, 1, 
        juce::dsp::Oversampling<sample_t>::FilterType::filterHalfBandPolyphaseIIR
    );

    driveType = CUBIC;
}

PreampDistorsion::~PreampDistorsion() {
    delete overSampler;
}

void PreampDistorsion::prepareToPlay(double _samplerate, int blockSize) {
    samplerate = _samplerate;

    preGain.init(samplerate, 0.02, 0.0, SMOOTH_PARAM_LIN);
    postGain.init(samplerate, 0.02, 0.0, SMOOTH_PARAM_LIN);

    inputFilter.prepareToPlay(samplerate);
    inputFilter.setCoefficients(inputFilterFrequency);

    overSampler->initProcessing(blockSize);

    preGain.newTarget(DB_TO_GAIN(0.0));
    postGain.newTarget(DB_TO_GAIN(-12.0));

    stageGain = DB_TO_GAIN(20.0);
}

void PreampDistorsion::process(float *input, size_t nSamples) {

    inputFilter.processBufferHighpass(input, nSamples);
    
    // d'abord sans upsampling pour voir si la courbe fonctionne bien.
        
    for (size_t index = 0; index < nSamples; index++) {
        
        input[index] = preGain.nextValue() * input[index];

        input[index] = stageGain * input[index];
        input[index] = processDrive(input[index], driveType);

        input[index] = stageGain * input[index];
        input[index] = processDrive(input[index], driveType);

        input[index] = postGain.nextValue() * input[index];
    }
}

sample_t PreampDistorsion::processDrive(sample_t sample, DriveType curveType) {
    
    switch (curveType) {
        case APPROX:
            sample = tanhApprox(sample);
            break;

        case TANH:
            sample = tanh(sample);
            break;

        case CUBIC: 
            sample = sample <= -1 ? -0.666f 
                   : sample >= 1 ? 0.666f 
                   : sample - 0.333 * pow(sample, 3);
            break;

        case HARDCLIP:
            break;
    }

    return sample > 1.0f ? 1.0f
         : sample < -1.0f ? -1.0f
         : sample;
}
