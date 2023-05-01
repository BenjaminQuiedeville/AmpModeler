/*
  ==============================================================================

    Biquad.cpp
    Created: 23 Apr 2023 12:01:44am
    Author:  Benjamin

  ==============================================================================
*/

#include "Biquad.h"

void Biquad::prepareToPlay(juce::dsp::ProcessSpec &spec) {
    samplerate = spec.sampleRate;
    twoPiOverSamplerate = juce::MathConstants<double>::twoPi / samplerate;

    b0 = 1.0f;
    b1 = 0.0f;
    b2 = 0.0f;
    a1 = 0.0f;
    a2 = 0.0f;
    reset();
}

void Biquad::reset() {
    x1 = 0.0f;
    x2 = 0.0f;
    y1 = 0.0f;
    y2 = 0.0f;
}

void Biquad::setCoefficients(const float frequency, const float Q, const float gaindB) {

    const float w0 = twoPiOverSamplerate * frequency;
    const float cosw0 = cos(w0);
    const float sinw0 = sin(w0);

    const float alpha = sinw0/(2.0f*Q);

    float A = 0.0f;
    float AInv = 0.0f; 
    float a0Inv = 0.0f;
    float twoSqrtAAlpha = 0.0f;

    switch (filterType) {
        case LOWPASS:
            a0Inv = 1.0f/(1.0f + alpha);

            b0 = (1.0f - cosw0) * 0.5f * a0Inv;
            b1 = 2.0f * b0;
            b2 = b0;
            a1 = -2.0f * cosw0 * a0Inv;
            a2 = (1.0f - alpha) * a0Inv;
            break;

        case HIGHPASS:
            a0Inv = 1.0f/(1.0f + alpha);

            b0 = (1.0f + cosw0) * 0.5f * a0Inv;
            b1 = -2.0f * b0;
            b2 = b0;
            a1 = -2.0f * cosw0 * a0Inv;
            a2 = (1.0f - alpha) * a0Inv;
            break;

        case PEAK: 
            A = pow(10.0f, gaindB/40.0f);
            AInv = 1.0f/A;
            a0Inv = 1.0f/(1.0f + alpha * AInv);

            b0 = (1.0f + alpha * A) * a0Inv;
            b1 = -2.0f * cosw0 * a0Inv; 
            b2 = (1.0f - alpha * A) * a0Inv; 
            a1 = b1; 
            a2 = (1.0f - alpha * AInv) * a0Inv;
            break;

        case LOWSHELF: 
            A = pow(10.0f, gaindB/40.0f);
            twoSqrtAAlpha = 2.0f * sqrt(A)* alpha;
            a0Inv = 1.0f/((A + 1.0f) + (A - 1.0f)*cosw0 + twoSqrtAAlpha); 
            
            b0 = A*((A + 1.0f) - (A - 1.0f)*cosw0 + twoSqrtAAlpha)*a0Inv;
            b1 = 2.0f * A*((A - 1.0f) - (A + 1.0f)*cosw0)*a0Inv;
            b2 = A*((A + 1.0f) - (A - 1.0f)*cosw0 - twoSqrtAAlpha) * a0Inv;
            a1 = -2.0f * ((A - 1.0f) + (A + 1.0f)*cosw0)*a0Inv;
            a2 = ((A + 1.0f) + (A - 1.0f)*cosw0 - twoSqrtAAlpha) * a0Inv;
            break;

        case HIGHSHELF:
            A = pow(10.0f, gaindB/40.0f);
            twoSqrtAAlpha = 2.0f * sqrt(A)* alpha;
            a0Inv = 1.0f/((A + 1.0f) - (A - 1.0f)* cosw0 + twoSqrtAAlpha); 

            b0 = A*((A + 1.0f) + (A - 1.0f) * cosw0 + twoSqrtAAlpha) * a0Inv;
            b1 = -2.0f * A *((A - 1.0f) + (A + 1.0f) * cosw0) * a0Inv;
            b2 = A*((A + 1.0f) + (A - 1.0f)*cosw0 - twoSqrtAAlpha) * a0Inv;
            a1 = 2.0f * ((A - 1.0f) - (A + 1.0f)*cosw0) * a0Inv;
            a2 = ((A + 1.0f) - (A - 1.0f)*cosw0 - twoSqrtAAlpha) * a0Inv;
            break;

        default: 
            b0 = 1.0f;
            b1 = 0.0f; 
            b2 = 0.0f; 
            a1 = 0.0f; 
            a2 = 0.0f;        
            break;
    }

    // x1 = 0.0f;
    // x2 = 0.0f;
    // y1 = 0.0f;
    // y2 = 0.0f;

}

void Biquad::process(sample_t *sample) {

    sample_t outputSample = *sample*b0 + x1*b1 + x2*b2
                                      - y1*a1 - y2*a2;
    
    x2 = x1;
    x1 = *sample; 
    y2 = y1; 
    y1 = outputSample;
    *sample = outputSample; 
}
