module toneStackSim

# David Te-Mao Yeh Thesis 2009

using ACME 
using Plots
using DSP
using Statistics
using FFTW 
using Debugger


abstract type Filter end

mutable struct Filter3{T <: Number} <: Filter

    B0::T
    B1::T
    B2::T 
    B3::T

    A1::T
    A2::T
    A3::T

    x1::T
    x2::T
    x3::T

    y1::T
    y2::T
    y3::T

end 

mutable struct Onepole <: Filter

    B0::Number
    A1::Number 

    Y1::Number
end

Onepole(freq::Number, samplerate::Number) = Onepole(sin(π/samplerate * freq), sin(π/samplerate * freq) - 1, 0.0)

Filter3{T}(B0, B1, B2, B3, A0, A1, A2, A3) where {T <: Number} = 
    Filter3{T}(B0/A0, B1/A0, B2/A0, B3/A0, A1/A0, A2/A0, A3/A0,
              0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

num(filter::Filter3)::Vector{Number}   = [filter.B0, filter.B1, filter.B2, filter.B3]
denum(filter::Filter3)::Vector{Number} = [1.0, filter.A1, filter.A2, filter.A3]


struct Components{T <: Number}

    R1::T
    R2::T
    R3::T
    R4::T

    C1::T
    C2::T
    C3::T
end 

function filter!(signal::Array, filter::Onepole)::Array
    output = zeros(size(signal))

    for i in 1:size(signal)[1]
        output[i] = (filter.B0 * signal[i] 
                  -  filter.A1 * filter.Y1)
        filter.Y1 = output[i]
    end 
    return output
end 

function filter!(signal::Vector, filter::Filter3)::Array

    output = zeros(Float64, size(signal))

    for i in 1:length(signal)
        output[i] = (filter.B0 * signal[i]
                   + filter.B1 * filter.x1
                   + filter.B2 * filter.x2
                   + filter.B3 * filter.x3
                   - filter.A1 * filter.y1
                   - filter.A2 * filter.y2
                   - filter.A3 * filter.y3)
        
        filter.x3 = filter.x2
        filter.x2 = filter.x1
        filter.x1 = signal[i]

        filter.y3 = filter.y2
        filter.y2 = filter.y1
        filter.y1 = output[i]
    end

    return output
end


function toneStackRational(cp::Components, t::Number, m::Number, L::Number, samplerate::Number, T::Type = Float64)::Filter

    l :: Float64 = exp((L-1)*3.4)

    b1::T = (t*cp.C1*cp.R1 + m*cp.C3*cp.R3 + l*(cp.C1*cp.R2 + cp.C2*cp.R2) + (cp.C1*cp.R3 + cp.C2*cp.R3))

    b2::T = (t*(cp.C1*cp.C2*cp.R1*cp.R4 + cp.C1*cp.C3*cp.R1*cp.R4) 
          - m*m*(cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3)
          + m*(cp.C1*cp.C3*cp.R1*cp.R3 + cp.C1*cp.C3*cp.R3*cp.R3 + cp.C2*cp.C3*cp.R3*cp.R3)
          + l*(cp.C1*cp.C2*cp.R1*cp.R2 + cp.C1*cp.C2*cp.R2*cp.R4 + cp.C1*cp.C3*cp.R2*cp.R4)
          + l*m*(cp.C1*cp.C3*cp.R2*cp.R3 + cp.C2*cp.C3*cp.R2*cp.R3)
          + (cp.C1*cp.C2*cp.R1*cp.R3 + cp.C1*cp.C2*cp.R3*cp.R4 + cp.C1*cp.C3*cp.R3*cp.R4))

    b3::T = (l*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R2*cp.R3*cp.R4)
          - m*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4)
          + m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4)
          + t*cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4 - t*m*cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4
          + t*l*cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R4)

    a0::T = 1

    a1::T = ((cp.C1*cp.R1 + cp.C1*cp.R3 + cp.C2*cp.R3 + cp.C2*cp.R4 + cp.C3*cp.R4)
          + m*cp.C3*cp.R3 + l*(cp.C1*cp.R2 + cp.C2*cp.R2))

    a2::T = (m*(cp.C1*cp.C3*cp.R1*cp.R3 
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
             + cp.C2*cp.C3*cp.R3*cp.R4))

    a3::T = (l*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R2*cp.R3*cp.R4)
          - m*m*(cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 + cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4)
          + m*(cp.C1*cp.C2*cp.C3*cp.R3*cp.R3*cp.R4 
             + cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R3 
             - cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4)
          + l*cp.C1*cp.C2*cp.C3*cp.R1*cp.R2*cp.R4
          + cp.C1*cp.C2*cp.C3*cp.R1*cp.R3*cp.R4)

    c = 2*samplerate

    B0::T = -b1*c - b2*c^2 - b3*c^3
    B1::T = -b1*c + b2*c^2 + 3*b3*c^3
    B2::T = b1*c + b2*c^2 - 3*b3*c^3
    B3::T = b1*c - b2*c^2 + b3*c^3
    A0::T = -a0 - a1*c - a2*c^2 - a3*c^3
    A1::T = -3*a0 - a1*c + a2*c^2 + 3*a3*c^3
    A2::T = -3*a0 + a1*c + a2*c^2 - 3*a3*c^3
    A3::T = -a0 + a1*c - a2*c^2 + a3*c^3

    return Filter3{T}(B0, B1, B2, B3, A0, A1, A2, A3)
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


function main()::Nothing
    
    t::Float64 = 0.5
    m::Float64 = 0.5
    l::Float64 = 0.5

    toneStackCustom     = Components{Float64}(250e3, 1e6, 20e3, 60e3, 0.60e-9, 35e-9, 40e-9)
    toneStackEnglSavage = Components{Float64}(250e3, 1e6, 20e3, 47e3, 0.47e-9, 47e-9, 22e-9)
    toneStackJCM800     = Components{Float64}(220e3, 1e6, 22e3, 33e3, 0.47e-9, 22e-9, 22e-9)
    toneStackSoldano    = Components{Float64}(250e3, 1e6, 25e3, 47e3, 0.47e-9, 20e-9, 20e-9)
    toneStackRectifier  = Components{Float64}(250e3, 1e6, 25e3, 47e3, 0.50e-9, 20e-9, 20e-9)

    samplerate::Float64 = 48000.0
    model = DiscreteModel(toneStack(toneStackCustom, t, m, l), 1/samplerate)
    
    nFreq = 2^14
    signal = [1; zeros(nFreq - 1)]'
    y = run!(model, signal)[1, :]

    ySpectre = 20 * log10.((y |> rfft .|> abs).^2)
    yFreqs = rfftfreq(size(y)[1], samplerate)

    ###

    toneJCM = toneStackRational(toneStackJCM800, t, m, l, samplerate, Float64)
    toneSoldano = toneStackRational(toneStackSoldano, t, m, l, samplerate, Float64)
    toneEngl = toneStackRational(toneStackEnglSavage, t, m, l, samplerate, Float64)
    toneCustom = toneStackRational(toneStackCustom, t, m, l, samplerate, Float64)
    toneRectifier = toneStackRational(toneStackRectifier, t, m, l, samplerate, Float64)
    testFilter = Onepole(1000, samplerate)

    signal = zeros(nFreq)
    signal[1] = 1
    yJCM = filter!(signal, toneJCM)
    ySoldano = filter!(signal, toneSoldano)
    yEngl = filter!(signal, toneEngl)
    yCustom = filter!(signal, toneCustom)
    yRectifier = filter!(signal, toneRectifier)
    @show toneJCM

    yJCMSpectre = 20 * log10.((yJCM |> rfft .|> abs).^2)
    ySoldanoSpectre = 20 * log10.((ySoldano |> rfft .|> abs).^2)
    yEnglSpectre = 20 * log10.((yEngl |> rfft .|> abs).^2)
    yCustomSpectre = 20 * log10.((yCustom |> rfft .|> abs).^2)
    yRectifierSpectre = 20 * log10.((yRectifier |> rfft .|> abs).^2)


    # p1 = begin
    #     plot()
    #     plot!(yFreqs .+ 1, ySpectre, 
    #           xaxis = :log, 
    #           ylims = [-50, 10])
    #     # plot!(y)
    # end

    p2 = begin
        plot()
        # plot!(yFreqs .+ 1, yJCMSpectre, 
        #       label = "JCM800")
        plot!(yFreqs .+ 1, ySoldanoSpectre,
              label = "Soldano")
        # plot!(yFreqs .+ 1, yEnglSpectre,
        #       label = "Engl")
        # plot!(yFreqs .+ 1, yCustomSpectre,
        #       label = "Custom settings")
        plot!(yFreqs .+ 1, yRectifierSpectre, 
              label = "Rectifier")
        
        plot!(xaxis = :log, 
              ylims = [-40, 10],
              xlims = [10, 10000],
              minorticks = 10,
              minorgrid = true,
              xlabel = "Frequence Hz", 
              ylabel = "Amplitude dB")
        # plot!(y2)
    end 

    # p3 = plot(p1, p2, layout = (2, 1)) |> display
    display(p2)

    return nothing
end

end # toneStackSim
import .toneStackSim as T

DEBUG::Bool = false

DEBUG && VSCodeServer.@run T.main()
!DEBUG && T.main()
