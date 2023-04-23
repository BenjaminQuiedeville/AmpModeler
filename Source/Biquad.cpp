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
    twoPiOverSampelrate = juce::MathConstants<float>::twoPi / samplerate;

    x1 = 0.0f;
    x2 = 0.0f;
    y1 = 0.0f;
    y2 = 0.0f;

    b0 = 1.0f;
    b1 = 0.0f; 
    b2 = 0.0f; 
    a1 = 0.0f; 
    a2 = 0.0f;   
}

void Biquad::setCoefficient(float frequency, float Q, float gaindB) {

    const float w0 = twoPiOverSampelrate * frequency;
    const float cosw0 = cos(w0);
    const float sinw0 = sin(w0);

    const float alpha = sinw0/(2.0f*Q);


    switch (filterType) {
        case LOWPASS:
            const float a0inv = 1 + alpha;

            b0 = (1.0f - cosw0) * 0.5f * a0inv;
            b1 = 2.0f * b0;
            b2 = b0;
            a1 = -2.0f * cosw0 * a0inv;
            a2 = (1.0f - alpha) * a0inv;
            break;

        case HIGHPASS:
            const float a0inv = 1 + alpha;

            b0 = (1.0f + cosw0) * 0.5f * a0inv;
            b1 = -2.0f * b0;
            b2 = b0;
            a1 = -2.0f * cosw0 * a0inv;
            a2 = (1.0f - alpha) * a0inv;
            break;

        case PEAK: 
            const float A = pow(10, gaindB/40);
            const float AInv = 1/A;
            const float a0Inv = 1.0f/(1.0f + alpha * AInv);

            b0 = (1.0f + alpha * A) * a0Inv;
            b1 = -2.0f * cosw0 * a0Inv; 
            b2 = (1.0f - alpha * A) * a0Inv; 
            a1 = b1; 
            a2 = (1.0f - alpha * AInv) * a0inv;
            break;

        case LOWSHELF: 
            const float A = pow(10, gaindB/40);
            const float twoSqrtAAlpha = 2.0f * sqrt(A)* alpha;
            const float a0Inv = 1.0f/(A + 1.0f) + (A - 1.0f)*cosw0 + twoSqrtAAlpha; 
            
            b0 = A*((A + 1.0f) - (A - 1.0f)*cosw0 + twoSqrtAAlpha)*a0Inv;
            b1 = 2.0f * A*((A - 1.0f) - (A + 1.0f)*cosw0)*a0Inv;
            b2 = A*((A + 1.0f) - (A - 1.0f)*cosw0 - twoSqrtAAlpha) * a0Inv;
            a1 = -2.0f * ((A - 1.0f) + (A + 1.0f)*cosw0)*a0Inv;
            a2 = ((A + 1.0f) + (A - 1.0f)*cosw0 - twoSqrtAAlpha) * a0Inv;
            break;

        case HIGHSHELF:
            const float A = pow(10, gaindB/40);
            const float twoSqrtAAlpha = 2.0f * sqrt(A)* alpha;
            const float a0Inv = 1.0f/(A + 1.0f) - (A - 1.0f)* cosw0 + twoSqrtAAlpha; 

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

}

float Biquad::process(float& sample) {
    float outputSample = sample * b0 + x1 * b1+ x2 * b2
                                     - y1 * a1 - y2 * a2;
    
    x2 = x1;
    x1 = sample; 
    y2 = y1; 
    y1 = outputSample;
    
    return outputSample;
}
