module toneStackSim

using ACME 
using Plots
using DSP

R1val :: Number = 150e3
R2val :: Number = 150e3
R3val :: Number = 10e3

toneStack(l :: Number = 0.5, 
          m :: Number = 0.5, 
          t :: Number = 0.5) = 
    @circuit begin
    Vi = voltagesource()
    R11 = resistor((1 - t)*R1val)
    R12 = resistor(t*R1val)
    R2 = resistor(l*R2val)
    R3 = resistor(m*R3val)
    R4 = resistor(100e3)

    C1 = capacitor(250e-12)
    C2 = capacitor(100e-9)
    C3 = capacitor(47e-9)

    Vout = voltageprobe()

    Vi[+] == R4[1] == C1[1]
    Vi[-] == gnd
    
    C1[2] == R11[1]
    R11[2] == Vout == R12[1]

    R4[2] == C2[1] == C3[1]
    C2[2] == R12[2] == R2[1]
    C3[2] == R2[2] == R3[1]
    R3[2] == gnd

end

function main() :: Nothing 
    samplerate :: Float64 = 48000.0
    model = DiscreteModel(toneStack(0.5, 0.5, 0.5), 1/samplerate)
    
    duree = 0.2 
    t = LinRange(0, duree, Int(samplerate*duree))
    signal :: Matrix{Float64}= Matrix(undef, 1, length(t))
    signal[1, :] = sinpi.(2*200*t)

    y = run!(model, signal)

    p1 = begin
        plot()
        plot!(t, y)
    end
    display(p1)


    return nothing 
end
main()

end # toneStackSim
