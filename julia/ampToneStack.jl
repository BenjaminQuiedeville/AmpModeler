module toneStackSim

# David Te-Mao Yeh Thesis 2009

using ACME 
using Plots
using DSP
using Statistics
using FFTW 


mutable struct Filter

    B0 :: Float64
    B1 :: Float64
    B2 :: Float64 
    B3 :: Float64

    A1 :: Float64
    A2 :: Float64
    A3 :: Float64

    x1 :: Float64
    x2 :: Float64
    x3 :: Float64

    y1 :: Float64
    y2 :: Float64
    y3 :: Float64

    Filter(B0, B1, B2, B3, A0, A1, A2, A3) = 
        new(B0/A0, B1/A0, B2/A0, B3/A0, A1/A0, A2/A0, A3/A0,
            0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

end 


struct Components

    R1 :: Float64
    R2 :: Float64
    R3 :: Float64
    R4 :: Float64

    C1 :: Float64
    C2 :: Float64
    C3 :: Float64
end 

function filter!(signal :: Array, filter :: Filter) :: Array

    output = zeros(size(signal))

    for i in size(signal)[1]
        output[i] = filter.B0 * signal[i]
                  + filter.B1 * filter.x1
                  + filter.B2 * filter.x2
                  + filter.B3 * filter.x3
                  - filter.A1 * filter.y1
                  - filter.A2 * filter.y2
                  - filter.A3 * filter.y3
        
        filter.x3 = filter.x2
        filter.x2 = filter.x1
        filter.x1 = signal[i]

        filter.y3 = filter.y2
        filter.y2 = filter.y1
        filter.y1 = output[i]
    end

    return output
end

function toneStackRational(comp::Components, t::Number, m::Number, l::Number, samplerate::Number)::Filter

    L = exp((l-1)*3.4)

    b1 = t*comp.C1*comp.R1 
        + m*comp.C3*comp.R3 
        + L*(comp.C1*comp.R2 + comp.C2*comp.R2) 
        + (comp.C1*comp.R3 + comp.C2*comp.R3);

    b2 = t*(comp.C1*comp.C2*comp.R1*comp.R4 + comp.C1*comp.C3*comp.R1*comp.R4) 
        - m*m*(comp.C1*comp.C3*comp.R3*comp.R3 + comp.C2*comp.C3*comp.R3*comp.R3)
        + m*(comp.C1*comp.C3*comp.R1*comp.R3 + comp.C1*comp.C3*comp.R3*comp.R3 + comp.C2*comp.C3*comp.R3*comp.R3)
        + L*(comp.C1*comp.C2*comp.R1*comp.R2 + comp.C1*comp.C2*comp.R2*comp.R4 + comp.C1*comp.C3*comp.R2*comp.R4)
        + L*m*(comp.C1*comp.C3*comp.R2*comp.R3 + comp.C2*comp.C3*comp.R2*comp.R3)
        + (comp.C1*comp.C2*comp.R1*comp.R3 + comp.C1*comp.C2*comp.R3*comp.R4 + comp.C1*comp.C3*comp.R3*comp.R4);

    b3 = L*m*(comp.C1*comp.C2*comp.C3*comp.R1*comp.R2*comp.R3 + comp.C1*comp.C2*comp.C3*comp.R2*comp.R3*comp.R4)
         - m*m*(comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R3 + comp.C1*comp.C2*comp.C3*comp.R3*comp.R3*comp.R4)
         + m*(comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R3 + comp.C1*comp.C2*comp.C3*comp.R3*comp.R3*comp.R4)
         + t*comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R4 
         - t*m*comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R4
         + t*L*comp.C1*comp.C2*comp.C3*comp.R1*comp.R2*comp.R4;

    a0 = 1;

    a1 = (comp.C1*comp.R1 + comp.C1*comp.R3 + comp.C2*comp.R3 + comp.C2*comp.R4 + comp.C3*comp.R4)
         + m*comp.C3*comp.R3 + L*(comp.C1*comp.R2 + comp.C2*comp.R2);

    a2 = m*(comp.C1*comp.C3*comp.R1*comp.R3 
            - comp.C2*comp.C3*comp.R3*comp.R4 
            + comp.C1*comp.C3*comp.R3*comp.R3 
            + comp.C2*comp.C3*comp.R3*comp.R3)
        + L*m*(comp.C1*comp.C3*comp.R2*comp.R3 + comp.C2*comp.C3*comp.R2*comp.R3)
        - m*m*(comp.C1*comp.C3*comp.R3*comp.R3 + comp.C2*comp.C3*comp.R3*comp.R3)
        + L*(comp.C1*comp.C2*comp.R2*comp.R4 
            + comp.C1*comp.C2*comp.R1*comp.R2 
            + comp.C1*comp.C3*comp.R2*comp.R4 
            + comp.C2*comp.C3*comp.R2*comp.R4)
        + (comp.C1*comp.C2*comp.R1*comp.R4 
            + comp.C1*comp.C3*comp.R1*comp.R4 
            + comp.C1*comp.C2*comp.R3*comp.R4 
            + comp.C1*comp.C2*comp.R1*comp.R3 
            + comp.C1*comp.C3*comp.R3*comp.R4 
            + comp.C2*comp.C3*comp.R3*comp.R4);

    a3 = L*m*(comp.C1*comp.C2*comp.C3*comp.R1*comp.R2*comp.R3 + comp.C1*comp.C2*comp.C3*comp.R2*comp.R3*comp.R4)
         - m*m*(comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R3 + comp.C1*comp.C2*comp.C3*comp.R3*comp.R3*comp.R4)
         + m*(comp.C1*comp.C2*comp.C3*comp.R3*comp.R3*comp.R4 
            + comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R3 
            - comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R4)
         + L*comp.C1*comp.C2*comp.C3*comp.R1*comp.R2*comp.R4
         + comp.C1*comp.C2*comp.C3*comp.R1*comp.R3*comp.R4;

    c = 2*samplerate;

    B0 = -b1*c - b2*c^2 - b3*c^3
    B1 = -b1*c + b2*c^2 + 3*b3*c^3
    B2 = b1*c + b2*c^2 - 3*b3*c^3
    B3 = b1*c - b2*c^2 + b3*c^3

    A0 = -a0 - a1*c - a2*c^2 - a3*c^3
    A1 = -3*a0 - a1*c + a2*c^2 + 3*a3*c^3
    A2 = -3*a0 + a1*c + a2*c^2 - 3*a3*c^3
    A3 = -a0 + a1*c - a2*c^2 + a3*c^3

    return Filter(B0, B1, B2, B3, A0, A1, A2, A3)
end 


R1val :: Number = 150e3
R2val :: Number = 150e3
R3val :: Number = 10e3

function toneStack(l = 0.5, m = 0.5, t = 0.5)
    circ = @circuit begin
        Vi = voltagesource()
        R11 = resistor((1 - t)*R1val)
        R12 = resistor(t*R1val)
        R2 = resistor(l*R2val)
        R31 = resistor((1-m)*R3val)
        R32 = resistor(m*R3val)
        R4 = resistor(100e3)

        C1 = capacitor(250e-12)
        C2 = capacitor(100e-9)
        C3 = capacitor(47e-9)

        Vout = voltageprobe()

        Vi[+] == R4[1] == C1[1]
        Vi[-] == gnd

        C1[2] == R11[1]
        R11[2] == R12[1] == Vout[+]
        Vout[-] == gnd

        R4[2] == C2[1] == C3[1]
        C2[2] == R12[2] == R2[1]
        R2[2] == R31[1]
        C3[2] == R31[2] == R32[1]
        R32[2] == gnd
    end
    return circ
end


function main() :: Nothing
    
    samplerate :: Float64 = 48000.0
    model = DiscreteModel(toneStack(0.5, 0.5, 0.5), 1/samplerate)
    
    nFreq = 8192
    signal = u = [1; zeros(nFreq - 1)]'
    y = run!(model, signal)[1, :]

    ySpectre = 20 * log10.((y |> rfft .|> abs).^2)
    yFreqs = rfftfreq(size(y)[1], samplerate)

    ###

    toneStackJCM800  = Components(220e3, 1e6, 22e3, 33e3, 470e-12, 22e-9, 22e-9)
    toneStackBassman = Components(250e3, 1e6, 25e3, 56e3, 250e-12, 20e-9, 20e-9)

    toneFilter = toneStackRational(toneStackBassman, 0.5, 0.5, 0.5, samplerate)

    filterCoeffs = PolynomialRatio([toneFilter.B0, toneFilter.B1, toneFilter.B2, toneFilter.B3], 
                                   [1,             toneFilter.A1, toneFilter.A2, toneFilter.A3])

    H, w = DSP.freqresp(filterCoeffs)

    H = 20 * log10.(abs.(H))
    w *= samplerate/(2π)

    p1 = begin
        plot()
        plot!(yFreqs .+ 1, ySpectre, xaxis = :log)
    end

    p2 = begin
        plot()
        plot!(w .+ 1, H, xaxis = :log)
    end 

    p3 = plot(p1, p2, layout = (2, 1))
    display(p3)

    return nothing
end

end # toneStackSim
using Debugger
import .toneStackSim as T
T.main()