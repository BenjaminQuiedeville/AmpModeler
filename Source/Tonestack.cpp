/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "Tonestack.h"

void Tonestack::setModel(TonestackModel newModel) {
    
    if (newModel == model) { return; }
    
    switch (newModel) {
    case EnglSavage: {

        comps->R1 = 250e3;
        comps->R2 = 1e6;
        comps->R3 = 20e3;
        comps->R4 = 47e3;
        comps->C1 = 0.47e-9;
        comps->C2 = 47e-9;
        comps->C3 = 22e-9;    
        break;
    }
    
    case JCM800: {
        comps->R1 = 220e3;
        comps->R2 = 1e6;
        comps->R3 = 22e3;
        comps->R4 = 33e3;
        comps->C1 = 0.47e-9;
        comps->C2 = 22e-9;
        comps->C3 = 22e-9;    
        break; 
    }

    case Soldano: {
        comps->R1 = 250e3;
        comps->R2 = 1e6;
        comps->R3 = 25e3;
        comps->R4 = 47e3;
        comps->C1 = 0.47e-9;
        comps->C2 = 20e-9;
        comps->C3 = 20e-9;    
        break; 
    }

    case Rectifier: {
        comps->R1 = 250e3;
        comps->R2 = 1e6;
        comps->R3 = 25e3;
        comps->R4 = 47e3;
        comps->C1 = 0.50e-9;
        comps->C2 = 20e-9;
        comps->C3 = 20e-9;    
        break; 
    }

    case Orange: {
        comps->R1 = 250e3;
        comps->R2 = 300e3;
        comps->R3 = 25e3;
        comps->R4 = 39e3;
        comps->C1 = 0.56e-9;
        comps->C2 = 22e-9;
        comps->C3 = 22e-9;    
        break; 
    }

    case Custom: {

        comps->R1 = 250e3;
        comps->R2 = 1e6;
        comps->R3 = 20e3;
        comps->R4 = 47e3;
        comps->C1 = 0.47e-9;
        comps->C2 = 47e-9;
        comps->C3 = 22e-9;    
        break;
    }
    }
    model = newModel;
    updateConstants();
}



void Tonestack::updateConstants() {
    
    ctes->beta11 = comps->C1*comps->R1;
    ctes->beta12 = comps->C3*comps->R3;
    ctes->beta13 = comps->C1*comps->R2 + comps->C2*comps->R2;
    ctes->beta14 = comps->C1*comps->R3 + comps->C2*comps->R3;
    
    ctes->beta21 = comps->C1*comps->C2*comps->R1*comps->R4 + comps->C1*comps->C3*comps->R1*comps->R4;
    ctes->beta22 = comps->C1*comps->C3*comps->R3*comps->R3 + comps->C2*comps->C3*comps->R3*comps->R3;
    ctes->beta23 = comps->C1*comps->C3*comps->R1*comps->R3 + comps->C1*comps->C3*comps->R3*comps->R3 + comps->C2*comps->C3*comps->R3*comps->R3;
    ctes->beta24 = comps->C1*comps->C2*comps->R1*comps->R2 + comps->C1*comps->C2*comps->R2*comps->R4 + comps->C1*comps->C3*comps->R2*comps->R4;
    ctes->beta25 = comps->C1*comps->C3*comps->R2*comps->R3 + comps->C2*comps->C3*comps->R2*comps->R3;
    ctes->beta26 = comps->C1*comps->C2*comps->R1*comps->R3 + comps->C1*comps->C2*comps->R3*comps->R4 + comps->C1*comps->C3*comps->R3*comps->R4;

    ctes->beta31 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R2*comps->R3 + comps->C1*comps->C2*comps->C3*comps->R2*comps->R3*comps->R4;
    ctes->beta32 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R3 + comps->C1*comps->C2*comps->C3*comps->R3*comps->R3*comps->R4;
    ctes->beta33 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R3 + comps->C1*comps->C2*comps->C3*comps->R3*comps->R3*comps->R4;
    ctes->beta34 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R4;
    ctes->beta35 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R4;
    ctes->beta36 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R2*comps->R4;

    ctes->alpha11 = comps->C1*comps->R1 + comps->C1*comps->R3 + comps->C2*comps->R3 + comps->C2*comps->R4 + comps->C3*comps->R4;
    ctes->alpha12 = comps->C3*comps->R3;
    ctes->alpha13 = comps->C1*comps->R2 + comps->C2*comps->R2;

    ctes->alpha21 = comps->C1*comps->C3*comps->R1*comps->R3 
                 - comps->C2*comps->C3*comps->R3*comps->R4 
                 + comps->C1*comps->C3*comps->R3*comps->R3 
                 + comps->C2*comps->C3*comps->R3*comps->R3;
    
    ctes->alpha22 = comps->C1*comps->C3*comps->R2*comps->R3 + comps->C2*comps->C3*comps->R2*comps->R3;
    ctes->alpha23 = comps->C1*comps->C3*comps->R3*comps->R3 + comps->C2*comps->C3*comps->R3*comps->R3;
    ctes->alpha24 = comps->C1*comps->C2*comps->R2*comps->R4 
                 + comps->C1*comps->C2*comps->R1*comps->R2 
                 + comps->C1*comps->C3*comps->R2*comps->R4 
                 + comps->C2*comps->C3*comps->R2*comps->R4;
                 
    ctes->alpha25 = comps->C1*comps->C2*comps->R1*comps->R4 
                 + comps->C1*comps->C3*comps->R1*comps->R4 
                 + comps->C1*comps->C2*comps->R3*comps->R4 
                 + comps->C1*comps->C2*comps->R1*comps->R3 
                 + comps->C1*comps->C3*comps->R3*comps->R4 
                 + comps->C2*comps->C3*comps->R3*comps->R4;

    ctes->alpha31 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R2*comps->R3 + comps->C1*comps->C2*comps->C3*comps->R2*comps->R3*comps->R4;
    ctes->alpha32 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R3 + comps->C1*comps->C2*comps->C3*comps->R3*comps->R3*comps->R4;
    ctes->alpha33 = comps->C1*comps->C2*comps->C3*comps->R3*comps->R3*comps->R4 
                 + comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R3 
                 - comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R4;
    ctes->alpha34 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R2*comps->R4;
    ctes->alpha35 = comps->C1*comps->C2*comps->C3*comps->R1*comps->R3*comps->R4;

}



//@TODO refactor to be thread safe
//@TODO refactor to precompute  constant values on model change 
void Tonestack::updateCoefficients(float t, float m, float l, double samplerate) {

    l = scale_linear(l, 0.0f, 1.0f, -1.0f, 2.0f);

    double L = std::exp((l-1.0)*3.4);

    double B1 = t*ctes->beta11 + m*ctes->beta12 + L*ctes->beta13 + ctes->beta14;

    double B2 = t*ctes->beta21 
              - m*m*ctes->beta22
              + m*ctes->beta23
              + L*ctes->beta24
              + L*m*ctes->beta25
              + ctes->beta26;

    double B3 = L*m*ctes->beta31
              - m*m*ctes->beta32
              + m*ctes->beta33
              + t*ctes->beta34 - t*m*ctes->beta35
              + t*L*ctes->beta36;

    double A0 = 1.0;

    double A1 = ctes->alpha11
              + m*ctes->alpha12 + L*ctes->alpha13;

    double A2 = m*ctes->alpha21
              + L*m*ctes->alpha22
              - m*m*ctes->alpha23
              + L*ctes->alpha24
              + ctes->alpha25;

    double A3 = L*m*ctes->alpha31
              - m*m*ctes->alpha32
              + m*ctes->alpha33
              + L*ctes->alpha34
              + ctes->alpha35;

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
