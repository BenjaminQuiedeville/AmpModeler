module toneStackSim

# David Te-Mao Yeh Thesis 2009

using ACME 
using Plots
using DSP
using Statistics
using FFTW 


abstract type Filter end

mutable struct Filter3{T <: Number} <: Filter

    B0 :: T
    B1 :: T
    B2 :: T 
    B3 :: T

    A1 :: T
    A2 :: T
    A3 :: T

    x1 :: T
    x2 :: T
    x3 :: T

    y1 :: T
    y2 :: T
    y3 :: T

end 

mutable struct Onepole <: Filter

    B0 :: Number
    A1 :: Number 

    Y1 :: Number
end

Onepole(freq :: Number, samplerate :: Number) = Onepole(sin(π/samplerate * freq), sin(π/samplerate * freq) - 1, 0.0)

Filter3{T}(B0, B1, B2, B3, A0, A1, A2, A3) where {T <: Number} = 
    Filter3{T}(B0/A0, B1/A0, B2/A0, B3/A0, A1/A0, A2/A0, A3/A0,
              0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

num(filter :: Filter3) :: Vector{Number} = [filter.B0, filter.B1, filter.B2, filter.B3]
denum(filter :: Filter3) :: Vector{Number} = [1.0, filter.A1, filter.A2, filter.A3]


struct Components{T <: Number}

    R1 :: T
    R2 :: T
    R3 :: T
    R4 :: T

    C1 :: T
    C2 :: T
    C3 :: T
end 

function filter!(signal :: Array, filter :: Onepole) :: Array
    output = zeros(size(signal))

    for i in 1:size(signal)[1]
        output[i] = filter.B0 * signal[i] - filter.A1 * filter.Y1
        filter.Y1 = output[i]
    end 
    return output
end 

function filter!(signal :: Array, filter :: Filter3) :: Array

    output = zeros(size(signal))

    for i in 1:size(signal)[1]
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

function printConstants(cp::Components)::Nothing

    b1coeffs = [cp.C1*cp.R1, cp.C3*cp.R3, (cp.C1*cp.R2 + cp.C2*cp.R2), (cp.C1*cp.R3 + cp.C2*cp.R3)]
    b2coeffs = [(cp.C1*cp.C2*cp.R1*cp.R4 + cp.C1*cp.C3*cp.R1*cp.R4),
                (cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3),
                (cp.C1*cp.C3*cp.R1*cp.R3 + cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3),
                (cp.C1*cp.C2*cp.R1*cp.R2 + cp.C1*cp.C2*cp.R2*cp.R4 + cp.C1*cp.C3*cp.R2*cp.R4),
                (cp.C1*cp.C3*cp.R2*cp.R3 + cp.C2*cp.C3*cp.R2*cp.R3),
                (cp.C1*cp.C2*cp.R1*cp.R3 + cp.C1*cp.C2*cp.R3*cp.R4 + cp.C1*cp.C3*cp.R3*cp.R4)]
    b3coeffs = [(cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R2*cp.R3*cp.R4),
                (cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4), 
                (cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4), 
                cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4, 
                cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4, 
                cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R4]
    a1coeffs = [(cp.C1*cp.R1 + cp.C1*cp.R3 + cp.C2*cp.R3 + cp.C2*cp.R4 + cp.C3*cp.R4), 
                cp.C3*cp.R3, 
                (cp.C1*cp.R2 + cp.C2*cp.R2)]
    a2coeffs = [(cp.C1*cp.C3*cp.R1*cp.R3 - cp.C2*cp.C3*cp.R3*cp.R4 
                    + cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3), 
                (cp.C1*cp.C3*cp.R2*cp.R3 + cp.C2*cp.C3*cp.R2*cp.R3), 
                (cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3), 
                (cp.C1*cp.C2*cp.R2*cp.R4 + cp.C1*cp.C2*cp.R1*cp.R2 
                    + cp.C1*cp.C3*cp.R2*cp.R4 + cp.C2*cp.C3*cp.R2*cp.R4),
                (cp.C1*cp.C2*cp.R1*cp.R4 + cp.C1*cp.C3*cp.R1*cp.R4 
                    + cp.C1*cp.C2*cp.R3*cp.R4 + cp.C1*cp.C2*cp.R1*cp.R3 
                    + cp.C1*cp.C3*cp.R3*cp.R4 + cp.C2*cp.C3*cp.R3*cp.R4)]
    a3coeffs = [(cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R2*cp.R3*cp.R4), 
                (cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4), 
                (cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4 + cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 
                    - cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4), 
                cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R4, 
                cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4]

    @show b1coeffs
    @show b2coeffs
    @show b3coeffs
    @show a1coeffs
    @show a2coeffs
    @show a3coeffs

    return nothing
end 

function toneStackRational(cp::Components, t::Number, m::Number, L::Number, samplerate::Number, T::Type = Float64)::Filter

    l = exp((L-1)*3.4)

    b1::T = t*cp.C1*cp.R1 + m*cp.C3*cp.R3 + l*(cp.C1*cp.R2 + cp.C2*cp.R2) + (cp.C1*cp.R3 + cp.C2*cp.R3);

    b2::T = t*(cp.C1*cp.C2*cp.R1*cp.R4 + cp.C1*cp.C3*cp.R1*cp.R4) 
       - m*m*(cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3)
       + m*(cp.C1*cp.C3*cp.R1*cp.R3 + cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3)
       + l*(cp.C1*cp.C2*cp.R1*cp.R2 + cp.C1*cp.C2*cp.R2*cp.R4 + cp.C1*cp.C3*cp.R2*cp.R4)
       + l*m*(cp.C1*cp.C3*cp.R2*cp.R3 + cp.C2*cp.C3*cp.R2*cp.R3)
       + (cp.C1*cp.C2*cp.R1*cp.R3 + cp.C1*cp.C2*cp.R3*cp.R4 + cp.C1*cp.C3*cp.R3*cp.R4);

    b3::T = l*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R2*cp.R3*cp.R4)
         - m*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4)
         + m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4)
         + t*cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4 - t*m*cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4
         + t*l*cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R4;

    a0::T = 1;

    a1::T = (cp.C1*cp.R1 + cp.C1*cp.R3 + cp.C2*cp.R3 + cp.C2*cp.R4 + cp.C3*cp.R4)
         + m*cp.C3*cp.R3 + l*(cp.C1*cp.R2 + cp.C2*cp.R2);

    a2::T = m*(cp.C1*cp.C3*cp.R1*cp.R3 
            - cp.C2*cp.C3*cp.R3*cp.R4 
            + cp.C1*cp.C3*cp.R3*cp.R3 
            + cp.C2*cp.C3*cp.R3*cp.R3)
         + l*m*(cp.C1*cp.C3*cp.R2*cp.R3 + cp.C2*cp.C3*cp.R2*cp.R3)
         - m*m*(cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3)
         + l*(cp.C1*cp.C2*cp.R2*cp.R4 
            + cp.C1*cp.C2*cp.R1*cp.R2 
            + cp.C1*cp.C3*cp.R2*cp.R4 
            + cp.C2*cp.C3*cp.R2*cp.R4)
         + (cp.C1*cp.C2*cp.R1*cp.R4 
            + cp.C1*cp.C3*cp.R1*cp.R4 
            + cp.C1*cp.C2*cp.R3*cp.R4 
            + cp.C1*cp.C2*cp.R1*cp.R3 
            + cp.C1*cp.C3*cp.R3*cp.R4 
            + cp.C2*cp.C3*cp.R3*cp.R4);

    a3::T = l*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R2*cp.R3*cp.R4)
         - m*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4)
         + m*(cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4 
            + cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 
            - cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4)
         + l*cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R4
         + cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4;

    c = 2*samplerate;

    B0::T = -b1*c - b2*c^2 - b3*c^3;
    B1::T = -b1*c + b2*c^2 + 3*b3*c^3;
    B2::T = b1*c + b2*c^2 - 3*b3*c^3;
    B3::T = b1*c - b2*c^2 + b3*c^3;
    A0::T = -a0 - a1*c - a2*c^2 - a3*c^3;
    A1::T = -3*a0 - a1*c + a2*c^2 + 3*a3*c^3;
    A2::T = -3*a0 + a1*c + a2*c^2 - 3*a3*c^3;
    A3::T = -a0 + a1*c - a2*c^2 + a3*c^3;

    return Filter3{T}(B0, B1, B2, B3, A0, A1, A2, A3)
end 


R1val :: Number = 150e3
R2val :: Number = 150e3
R3val :: Number = 10e3

function toneStack(t = 0.5, m = 0.5, l = 0.5)
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
    
    t :: Float64 = 0.5
    m :: Float64 = 0.5
    l :: Float64 = 0.5 

    samplerate :: Float64 = 48000.0
    model = DiscreteModel(toneStack(t, m, l), 1/samplerate)
    
    nFreq = 8192
    signal = [1; zeros(nFreq - 1)]'
    y = run!(model, signal)[1, :]

    ySpectre = 20 * log10.((y |> rfft .|> abs).^2)
    yFreqs = rfftfreq(size(y)[1], samplerate)

    ###

    toneStackJCM800  = Components{Float64}(220e3, 1e6, 22e3, 33e3, 470e-12, 22e-9, 22e-9)
    toneStackBassman = Components{Float64}(250e3, 1e6, 25e3, 56e3, 250e-12, 20e-9, 20e-9)
    toneStackSoldano = Components{Float64}(250e3, 1e6, 25e3, 47e3, 470e-12, 20e-9, 20e-9)

    printConstants(toneStackBassman)

    toneFilter = toneStackRational(toneStackSoldano, t, m, l, samplerate, Float64)
    testFilter = Onepole(1/(0.3), samplerate)

    signal = randn(nFreq)
    signal[begin] = 1
    y2 = filter!(signal, toneFilter)

    y2Spectre = 20 * log10.((y2 |> rfft .|> abs).^2)



    p1 = begin
        plot()
        plot!(yFreqs .+ 1, ySpectre, xaxis = :log, ylims = [-60, -10])
    end

    p2 = begin
        plot()
        plot!(yFreqs .+ 1, y2Spectre, xaxis = :log)
    end 

    p3 = plot(p1, p2, layout = (2, 1)) |> display

    return nothing
end

end # toneStackSim
using Debugger
import .toneStackSim as T
T.main()