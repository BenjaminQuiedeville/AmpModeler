/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#include "Tonestack.h"

//@TODO refactor to be thread safe
//@TODO refactor to precompute  constant values on model change 
void Tonestack::updateCoefficients(float t, float m, float l, double samplerate) {

    // l = scale(l, 0.0f, 1.0f, -1.0f, 2.0f, 1.0f);

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
