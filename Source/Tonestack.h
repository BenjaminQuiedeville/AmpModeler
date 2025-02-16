/*
  ==============================================================================
    Author:  Benjamin Quiedeville
  ==============================================================================
*/

#ifndef TONE_STACK_H
#define TONE_STACK_H

#include "common.h"
#include "SmoothParam.h"
#include <memory>

enum TonestackModel {
    Soldano = 0,
    EnglSavage,
    JCM800,
    N_MODELS
};

struct Tonestack {

    Tonestack() {    
        setModel(EnglSavage);
    }

    void prepareToPlay() {
    
        bassParam.init(0.5);
        midParam.init(0.5);
        trebbleParam.init(0.5);
    
        updateCoefficients((float)trebbleParam.currentValue,
                            (float)midParam.currentValue, 
                            (float)bassParam.currentValue);
        
        x1L = 0.0f;
        x2L = 0.0f;
        x3L = 0.0f;
        y1L = 0.0f;
        y2L = 0.0f;
        y3L = 0.0f;
        
        x1R = 0.0f;
        x2R = 0.0f;
        x3R = 0.0f;
        y1R = 0.0f;
        y2R = 0.0f;
        y3R = 0.0f;    
    }


    void process(float *bufferL, float *bufferR, size_t nSamples) {
        ZoneScoped;
        bool update = false;
        
        if (bassParam.isSmoothing 
            || midParam.isSmoothing
            || trebbleParam.isSmoothing)
        {
            update = true;
        }
        
        float *buffers[2] = {bufferL, bufferR};
        u32 nChannels = bufferR ? 2 : 1;
        
        for (u32 channelIndex = 0; channelIndex < nChannels; channelIndex++) {
            for (u32 i = 0; i < nSamples; i++) {
            
                if (update) {
                    updateCoefficients((float)trebbleParam.nextValue(), 
                                       (float)midParam.nextValue(), 
                                       (float)bassParam.nextValue());
                }
            
                float outputSample = (float)(buffers[channelIndex][i] * b0
                                    + x1L * b1
                                    + x2L * b2
                                    + x3L * b3
                                    - y1L * a1
                                    - y2L * a2
                                    - y3L * a3);
        
                x3L = x2L; 
                x2L = x1L;
                x1L = bufferL[i];
                
                y3L = y2L; 
                y2L = y1L;
                y1L = outputSample;
        
                buffers[channelIndex][i] = outputSample;
            }
        }
    }
    
    void setModel(TonestackModel newModel) {
            
        switch (newModel) {
            case EnglSavage: {
                comps.R1 = 250e3;
                comps.R2 = 1e6;
                comps.R3 = 20e3;
                comps.R4 = 47e3;
                comps.C1 = 0.47e-9;
                comps.C2 = 47e-9;
                comps.C3 = 22e-9;    
                break;
            }
            
            case JCM800: {
                comps.R1 = 220e3;
                comps.R2 = 1e6;
                comps.R3 = 22e3;
                comps.R4 = 33e3;
                comps.C1 = 0.47e-9;
                comps.C2 = 22e-9;
                comps.C3 = 22e-9;    
                break; 
            }
        
            case Soldano: {
                comps.R1 = 250e3;
                comps.R2 = 1e6;
                comps.R3 = 25e3;
                comps.R4 = 47e3;
                comps.C1 = 0.47e-9;
                comps.C2 = 20e-9;
                comps.C3 = 20e-9;    
                break; 
            }
        }
        
        model = newModel;
        
        ctes.beta11 = comps.C1*comps.R1;
        ctes.beta12 = comps.C3*comps.R3;
        ctes.beta13 = comps.C1*comps.R2 + comps.C2*comps.R2;
        ctes.beta14 = comps.C1*comps.R3 + comps.C2*comps.R3;
        
        ctes.beta21 = comps.C1*comps.C2*comps.R1*comps.R4 + comps.C1*comps.C3*comps.R1*comps.R4;
        ctes.beta22 = comps.C1*comps.C3*comps.R3*comps.R3 + comps.C2*comps.C3*comps.R3*comps.R3;
        ctes.beta23 = comps.C1*comps.C3*comps.R1*comps.R3 + comps.C1*comps.C3*comps.R3*comps.R3 + comps.C2*comps.C3*comps.R3*comps.R3;
        ctes.beta24 = comps.C1*comps.C2*comps.R1*comps.R2 + comps.C1*comps.C2*comps.R2*comps.R4 + comps.C1*comps.C3*comps.R2*comps.R4;
        ctes.beta25 = comps.C1*comps.C3*comps.R2*comps.R3 + comps.C2*comps.C3*comps.R2*comps.R3;
        ctes.beta26 = comps.C1*comps.C2*comps.R1*comps.R3 + comps.C1*comps.C2*comps.R3*comps.R4 + comps.C1*comps.C3*comps.R3*comps.R4;
    
        ctes.beta31 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R2*comps.R3 + comps.C1*comps.C2*comps.C3*comps.R2*comps.R3*comps.R4;
        ctes.beta32 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R3 + comps.C1*comps.C2*comps.C3*comps.R3*comps.R3*comps.R4;
        ctes.beta33 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R3 + comps.C1*comps.C2*comps.C3*comps.R3*comps.R3*comps.R4;
        ctes.beta34 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R4;
        ctes.beta35 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R4;
        ctes.beta36 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R2*comps.R4;
    
        ctes.alpha11 = comps.C1*comps.R1 + comps.C1*comps.R3 + comps.C2*comps.R3 + comps.C2*comps.R4 + comps.C3*comps.R4;
        ctes.alpha12 = comps.C3*comps.R3;
        ctes.alpha13 = comps.C1*comps.R2 + comps.C2*comps.R2;
    
        ctes.alpha21 = comps.C1*comps.C3*comps.R1*comps.R3 
                     - comps.C2*comps.C3*comps.R3*comps.R4 
                     + comps.C1*comps.C3*comps.R3*comps.R3 
                     + comps.C2*comps.C3*comps.R3*comps.R3;
        
        ctes.alpha22 = comps.C1*comps.C3*comps.R2*comps.R3 + comps.C2*comps.C3*comps.R2*comps.R3;
        ctes.alpha23 = comps.C1*comps.C3*comps.R3*comps.R3 + comps.C2*comps.C3*comps.R3*comps.R3;
        ctes.alpha24 = comps.C1*comps.C2*comps.R2*comps.R4 
                     + comps.C1*comps.C2*comps.R1*comps.R2 
                     + comps.C1*comps.C3*comps.R2*comps.R4 
                     + comps.C2*comps.C3*comps.R2*comps.R4;
                     
        ctes.alpha25 = comps.C1*comps.C2*comps.R1*comps.R4 
                     + comps.C1*comps.C3*comps.R1*comps.R4 
                     + comps.C1*comps.C2*comps.R3*comps.R4 
                     + comps.C1*comps.C2*comps.R1*comps.R3 
                     + comps.C1*comps.C3*comps.R3*comps.R4 
                     + comps.C2*comps.C3*comps.R3*comps.R4;
    
        ctes.alpha31 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R2*comps.R3 + comps.C1*comps.C2*comps.C3*comps.R2*comps.R3*comps.R4;
        ctes.alpha32 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R3 + comps.C1*comps.C2*comps.C3*comps.R3*comps.R3*comps.R4;
        ctes.alpha33 = comps.C1*comps.C2*comps.C3*comps.R3*comps.R3*comps.R4 
                     + comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R3 
                     - comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R4;
        ctes.alpha34 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R2*comps.R4;
        ctes.alpha35 = comps.C1*comps.C2*comps.C3*comps.R1*comps.R3*comps.R4;
    
    }


    void updateCoefficients(float t, float m, float l) {
        ZoneScoped;    
        l = scale_linear(l, 0.0f, 1.0f, 0.0f, 1.0f);
        // m = scale_linear(m, 0.0f, 1.0f, 0.0f, 1.5f);
        // t = scale_linear(t, 0.0f, 1.0f, 0.0f, 1.0f);
        
        double L = std::exp((l-1.0)*3.4);
    
        double B1 = t*ctes.beta11 + m*ctes.beta12 + L*ctes.beta13 + ctes.beta14;
    
        double B2 = t*ctes.beta21 
                  - m*m*ctes.beta22
                  + m*ctes.beta23
                  + L*ctes.beta24
                  + L*m*ctes.beta25
                  + ctes.beta26;
    
        double B3 = L*m*ctes.beta31
                  - m*m*ctes.beta32
                  + m*ctes.beta33
                  + t*ctes.beta34 - t*m*ctes.beta35
                  + t*L*ctes.beta36;
    
        double A0 = 1.0;
    
        double A1 = ctes.alpha11
                  + m*ctes.alpha12 + L*ctes.alpha13;
    
        double A2 = m*ctes.alpha21
                  + L*m*ctes.alpha22
                  - m*m*ctes.alpha23
                  + L*ctes.alpha24
                  + ctes.alpha25;
    
        double A3 = L*m*ctes.alpha31
                  - m*m*ctes.alpha32
                  + m*ctes.alpha33
                  + L*ctes.alpha34
                  + ctes.alpha35;
    
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



    SmoothParamLinear bassParam;
    SmoothParamLinear midParam;
    SmoothParamLinear trebbleParam;

    double b0 = 1.0;
    double b1 = 0.0;
    double b2 = 0.0;
    double b3 = 0.0;

    double a1 = 0.0;
    double a2 = 0.0;
    double a3 = 0.0;

    float x1L = 0.0f;
    float x2L = 0.0f;
    float x3L = 0.0f;

    float y1L = 0.0f;
    float y2L = 0.0f;
    float y3L = 0.0f;

    float x1R = 0.0f;
    float x2R = 0.0f;
    float x3R = 0.0f;

    float y1R = 0.0f;
    float y2R = 0.0f;
    float y3R = 0.0f;
    
    double samplerate = 0.0;
    TonestackModel model;
    
    struct TonestackConstants {
        
        double beta11 = 0.0;
        double beta12 = 0.0;
        double beta13 = 0.0;
        double beta14 = 0.0;
        
        double beta21 = 0.0;
        double beta22 = 0.0;
        double beta23 = 0.0;
        double beta24 = 0.0;
        double beta25 = 0.0;
        double beta26 = 0.0;
        
        double beta31 = 0.0;
        double beta32 = 0.0;
        double beta33 = 0.0;
        double beta34 = 0.0;
        double beta35 = 0.0;
        double beta36 = 0.0;
    
        double alpha11 = 0.0;
        double alpha12 = 0.0;
        double alpha13 = 0.0;
        
        double alpha21 = 0.0;
        double alpha22 = 0.0;
        double alpha23 = 0.0;
        double alpha24 = 0.0;
        double alpha25 = 0.0;
        
        double alpha31 = 0.0;
        double alpha32 = 0.0;
        double alpha33 = 0.0;
        double alpha34 = 0.0;
        double alpha35 = 0.0;
    } ctes;

    struct TonestackComponents {
    
        double R1 = 0.0;
        double R2 = 0.0;
        double R3 = 0.0;
        double R4 = 0.0;
    
        double C1 = 0.0;
        double C2 = 0.0;
        double C3 = 0.0;
    } comps;
};

#endif // TONE_STACK_H




