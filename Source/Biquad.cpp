/*
  ==============================================================================

    Biquad.cpp
    Created: 17 Nov 2023 8:06:02pm
    Author:  benjamin

  ==============================================================================
*/

#include "Biquad.h"

void Biquad::setCoefficients(double frequency, double Q, double gaindB) {

    double w0 = twoPiOverSamplerate * frequency;
    double cosw0 = cos(w0);
    double sinw0 = sin(w0);

    double alpha = sinw0/(2.0f*Q);

    double A = 0.0;
    double AInv = 0.0; 
    double a0Inv = 0.0;
    double twoSqrtAlpha = 0.0;

    switch (filterType) {
        case BIQUAD_LOWPASS:
            a0Inv = 1.0/(1.0 + alpha);

            b0 = (1.0 - cosw0) * 0.5 * a0Inv;
            b1 = 2.0 * b0;
            b2 = b0;
            a1 = -2.0 * cosw0 * a0Inv;
            a2 = (1.0 - alpha) * a0Inv;
            break;

        case BIQUAD_HIGHPASS:
            a0Inv = 1.0/(1.0 + alpha);

            b0 = (1.0 + cosw0) * 0.5 * a0Inv;
            b1 = -2.0 * b0;
            b2 = b0;
            a1 = -2.0 * cosw0 * a0Inv;
            a2 = (1.0 - alpha) * a0Inv;
            break;

        case BIQUAD_PEAK: 
            A = pow(10.0, gaindB/40.0);
            AInv = 1.0/A;
            a0Inv = 1.0/(1.0 + alpha * AInv);

            b0 = (1.0 + alpha * A) * a0Inv;
            b1 = -2.0 * cosw0 * a0Inv; 
            b2 = (1.0 - alpha * A) * a0Inv; 
            a1 = b1; 
            a2 = (1.0 - alpha * AInv) * a0Inv;
            break;

        case BIQUAD_LOWSHELF: 
            A = pow(10.0, gaindB/40.0);
            twoSqrtAlpha = 2.0 * sqrt(A)* alpha;
            a0Inv = 1.0/((A + 1.0) + (A - 1.0)*cosw0 + twoSqrtAlpha); 
            
            b0 = A*((A + 1.0) - (A - 1.0)*cosw0 + twoSqrtAlpha)*a0Inv;
            b1 = 2.0 * A*((A - 1.0) - (A + 1.0)*cosw0)*a0Inv;
            b2 = A*((A + 1.0) - (A - 1.0)*cosw0 - twoSqrtAlpha) * a0Inv;
            a1 = -2.0 * ((A - 1.0) + (A + 1.0)*cosw0)*a0Inv;
            a2 = ((A + 1.0) + (A - 1.0)*cosw0 - twoSqrtAlpha) * a0Inv;
            break;

        case BIQUAD_HIGHSHELF:
            A = pow(10.0, gaindB/40.0);
            twoSqrtAlpha = 2.0 * sqrt(A)* alpha;
            a0Inv = 1.0/((A + 1.0) - (A - 1.0)* cosw0 + twoSqrtAlpha); 

            b0 = A*((A + 1.0) + (A - 1.0) * cosw0 + twoSqrtAlpha) * a0Inv;
            b1 = -2.0 * A *((A - 1.0) + (A + 1.0) * cosw0) * a0Inv;
            b2 = A*((A + 1.0) + (A - 1.0)*cosw0 - twoSqrtAlpha) * a0Inv;
            a1 = 2.0 * ((A - 1.0) - (A + 1.0)*cosw0) * a0Inv;
            a2 = ((A + 1.0) - (A - 1.0)*cosw0 - twoSqrtAlpha) * a0Inv;
            break;

        default:
            b0 = 1.0;
            b1 = 0.0;
            b2 = 0.0;
            a1 = 0.0;
            a2 = 0.0;
            break;
    }
}
