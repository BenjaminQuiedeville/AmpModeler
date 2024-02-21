# David Te-Mao Yeh Thesis 2009

using ACME
using FFTW
using DSP


mutable struct Components{T <: Number}

    R1::T
    R2::T
    R3::T
    R4::T

    C1::T
    C2::T
    C3::T
end 

function toneStack(cp::Components, t = 0.5, m = 0.5, l = 0.5)

    circ = @circuit begin
        Vi = voltagesource()
        R11 = resistor((1 - t)*cp.R1)
        R12 = resistor(t*cp.R1)
        R2 = resistor(l*cp.R2)
        R31 = resistor((1-m)*cp.R3)
        R32 = resistor(m*cp.R3)
        R4 = resistor(cp.R4)

        C1 = capacitor(cp.C1)
        C2 = capacitor(cp.C2)
        C3 = capacitor(cp.C3)

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

    model = DiscreteModel(toneStack(toneStackCustom, t, m, l), 1/samplerate)

    nFreq = 2^14
    signal = [1; zeros(nFreq - 1)]'
    y = run!(model, signal)[1, :]

    ySpectre = 20 * log10.((y |> rfft .|> abs).^2)
    yFreqs = rfftfreq(size(y)[1], samplerate) .+ 1.0


    return nothing    
end