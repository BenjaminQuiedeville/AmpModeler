/*
  ==============================================================================

    ThreeBandEQ.cpp
    Created: 23 Apr 2023 9:08:03pm
    Author:  Benjamin

  ==============================================================================
*/

#include "ThreeBandEQ.h"

void ToneStack::updateCoefficients(float t, float m, float l) {

    double L = std::exp((l-1.0)*3.4);

    double B1 = (t*comp->C1*comp->R1 + m*comp->C3*comp->R3 + L*(comp->C1*comp->R2 + comp->C2*comp->R2) + (comp->C1*comp->R3 + comp->C2*comp->R3));

    double B2 = (t*(comp->C1*comp->C2*comp->R1*comp->R4 + comp->C1*comp->C3*comp->R1*comp->R4) 
              - m*m*(comp->C1*comp->C3*comp->R3*comp->R3 + comp->C2*comp->C3*comp->R3*comp->R3)
              + m*(comp->C1*comp->C3*comp->R1*comp->R3 + comp->C1*comp->C3*comp->R3*comp->R3 + comp->C2*comp->C3*comp->R3*comp->R3)
              + L*(comp->C1*comp->C2*comp->R1*comp->R2 + comp->C1*comp->C2*comp->R2*comp->R4 + comp->C1*comp->C3*comp->R2*comp->R4)
              + L*m*(comp->C1*comp->C3*comp->R2*comp->R3 + comp->C2*comp->C3*comp->R2*comp->R3)
              + (comp->C1*comp->C2*comp->R1*comp->R3 + comp->C1*comp->C2*comp->R3*comp->R4 + comp->C1*comp->C3*comp->R3*comp->R4));

    double B3 = (L*m*(comp->C1*comp->C2*comp->C3*comp->R1*comp->R2*comp->R3 + comp->C1*comp->C2*comp->C3*comp->R2*comp->R3*comp->R4)
              - m*m*(comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R3 + comp->C1*comp->C2*comp->C3*comp->R3*comp->R3*comp->R4)
              + m*(comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R3 + comp->C1*comp->C2*comp->C3*comp->R3*comp->R3*comp->R4)
              + t*comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R4 - t*m*comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R4
              + t*L*comp->C1*comp->C2*comp->C3*comp->R1*comp->R2*comp->R4);

    double A0 = 1.0;

    double A1 = ((comp->C1*comp->R1 + comp->C1*comp->R3 + comp->C2*comp->R3 + comp->C2*comp->R4 + comp->C3*comp->R4)
              + m*comp->C3*comp->R3 + L*(comp->C1*comp->R2 + comp->C2*comp->R2));

    double A2 = (m*(comp->C1*comp->C3*comp->R1*comp->R3 
                 - comp->C2*comp->C3*comp->R3*comp->R4 
                 + comp->C1*comp->C3*comp->R3*comp->R3 
                 + comp->C2*comp->C3*comp->R3*comp->R3)
              + L*m*(comp->C1*comp->C3*comp->R2*comp->R3 + comp->C2*comp->C3*comp->R2*comp->R3)
              - m*m*(comp->C1*comp->C3*comp->R3*comp->R3 + comp->C2*comp->C3*comp->R3*comp->R3)
              + L*(comp->C1*comp->C2*comp->R2*comp->R4 
                 + comp->C1*comp->C2*comp->R1*comp->R2 
                 + comp->C1*comp->C3*comp->R2*comp->R4 
                 + comp->C2*comp->C3*comp->R2*comp->R4)
              + (comp->C1*comp->C2*comp->R1*comp->R4 
                 + comp->C1*comp->C3*comp->R1*comp->R4 
                 + comp->C1*comp->C2*comp->R3*comp->R4 
                 + comp->C1*comp->C2*comp->R1*comp->R3 
                 + comp->C1*comp->C3*comp->R3*comp->R4 
                 + comp->C2*comp->C3*comp->R3*comp->R4));

    double A3 = (L*m*(comp->C1*comp->C2*comp->C3*comp->R1*comp->R2*comp->R3 + comp->C1*comp->C2*comp->C3*comp->R2*comp->R3*comp->R4)
              - m*m*(comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R3 + comp->C1*comp->C2*comp->C3*comp->R3*comp->R3*comp->R4)
              + m*(comp->C1*comp->C2*comp->C3*comp->R3*comp->R3*comp->R4 
                 + comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R3 
                 - comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R4)
              + L*comp->C1*comp->C2*comp->C3*comp->R1*comp->R2*comp->R4
              + comp->C1*comp->C2*comp->C3*comp->R1*comp->R3*comp->R4);

    double c = 2.0*samplerate;


    double a0 = -A0 - A1*c - A2*std::pow(c, 2.0) - A3*std::pow(c, 3.0);

    b0 = (-B1*c - B2*std::pow(c, 2.0) - B3*std::pow(c, 3.0))/a0;
    b1 = (-B1*c + B2*std::pow(c, 2.0) + 3*B3*std::pow(c, 3.0))/a0;
    b2 = (B1*c + B2*std::pow(c, 2.0) - 3*B3*std::pow(c, 3.0))/a0;
    b3 = (B1*c - B2*std::pow(c, 2.0) + B3*std::pow(c, 3.0))/a0;
    a1 = (-3*A0 - A1*c + A2*std::pow(c, 2.0) + 3*A3*std::pow(c, 3.0))/a0;
    a2 = (-3*A0 + A1*c + A2*std::pow(c, 2.0) - 3*A3*std::pow(c, 3.0))/a0;
    a3 = (-A0 + A1*c - A2*std::pow(c, 2.0) + A3*std::pow(c, 3.0))/a0;

}



ThreeBandEQ::ThreeBandEQ() {

    bassFilters    = new Biquad(FilterType::BIQUAD_PEAK);
    midFilters     = new Biquad(FilterType::BIQUAD_PEAK);
    trebbleFilters = new Biquad(FilterType::BIQUAD_PEAK);
}

ThreeBandEQ::~ThreeBandEQ() {
    delete bassFilters;
    delete midFilters;
    delete trebbleFilters;
}

void ThreeBandEQ::prepareToPlay(double _samplerate) {

    bassFilters->prepareToPlay(_samplerate);
    midFilters->prepareToPlay(_samplerate);
    trebbleFilters->prepareToPlay(_samplerate);
}

void ThreeBandEQ::process(float *input, size_t nSamples) {

    bassFilters->process(input, nSamples);
    midFilters->process(input, nSamples);
    trebbleFilters->process(input, nSamples);
}

void ThreeBandEQ::updateGains(double bassGain, double midGain, double trebbleGain) {

    bassFilters->setCoefficients(bassFreq, bassQ, bassGain);
    midFilters->setCoefficients(midFreq, midQ, midGain);
    trebbleFilters->setCoefficients(trebbleFreq, trebbleQ, trebbleGain);
}
