declare name "TableSaw";
declare version "0.0.1";
declare author "FDN_Seeker";
declare description "Boss HM2 Emulation";

// compile with command 
// faust Tablesaw.dsp -o Tablesaw.cpp -lang cpp -nvi -vec -cn TablesawDSP

import("stdfaust.lib");
import("filter.lib");

gain = hslider("gain", 0.5, 0.0, 1.0, 0.1);
eq_low = hslider("low", 0.5, 0.0, 1.0, 0.1);
eq_high = hslider("high", 0.5, 0.0, 1.0, 0.1);
volume = hslider("vol", 0.5, 0.0, 1.0, 0.1);

// peak_eq_cq(boost_db, frequence, Q)
// highpass/lowpass(ordre, frequence)

// bypass(circuit, param) = _ <: circuit, _ : *(param) * (1 - param) : +;
// write_left_to_right(param) = (_ <: _, _), _ : _, (*(param), *(1-param) : +)

process = hm2
with {

    ceiling = 1.5;
    min_gain_db = 0.0;
    max_gain_db = 60.0;
    dist_gain = ba.db2linear(gain * max_gain_db);
    crossover_point = 0.01;

    asym_tanh = _ <: (min(0) : ma.tanh),
                     (min(1) : max(0)),
                     (max(1) : -(1) : ma.tanh) :
                     +, _ : +;

    crossover = _ <: (max(crossover_point) - crossover_point), 
                     (min(-crossover_point) + crossover_point) 
                     : + ;
  
    hard_clip = min(0.7) : max(-0.7);


    hm2 = *(dist_gain) 
        : /(ceiling) : asym_tanh : *(ceiling)

        : highpass(1, 10) 
        : crossover 
        
        : hard_clip 
        : lowpass(1, 12000) : highpass(1, 30)
    
        // faust seems to not compute filter frequencies properly at higher sampling rate
        : peak_eq_cq(eq_low * 20, 40, 0.5)    
        : peak_eq_cq(eq_high * 20, 750, 0.5)  
        : peak_eq_cq(eq_high * 20, 500, 0.5) 
        : *(ba.db2linear(-10))
        : *(volume*volume);
};
