using GLMakie
using DSP
using FFTW 

mutable struct Filter3{T <: Number}

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


Filter3{T}(B0, B1, B2, B3, A0, A1, A2, A3) where {T <: Number} = 
    Filter3{T}(B0/A0, B1/A0, B2/A0, B3/A0, A1/A0, A2/A0, A3/A0,
              0.0, 0.0, 0.0, 0.0, 0.0, 0.0)

num(filter::Filter3)::Vector{Number}   = [filter.B0, filter.B1, filter.B2, filter.B3]
denum(filter::Filter3)::Vector{Number} = [1.0, filter.A1, filter.A2, filter.A3]


mutable struct Components{T <: Number}

    R1::T
    R2::T
    R3::T
    R4::T

    C1::T
    C2::T
    C3::T
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


function toneStackRational(cp::Components, t::Number, m::Number, L::Number, samplerate::Number, T::Type = Float64)::Filter3

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



function main() :: Nothing
    t :: Float64 = 0.5
    m :: Float64 = 0.5
    l :: Float64 = 0.5

    toneStackCustom     = Components{Float64}(250e3, 1e6, 25e3, 47e3, 0.47e-9, 20e-9, 40e-9)
    toneStackEnglSavage = Components{Float64}(250e3, 1e6, 20e3, 47e3, 0.47e-9, 47e-9, 22e-9)
    
    toneStackSoldano    = Components{Float64}(250e3, 1e6, 25e3, 47e3, 0.47e-9, 20e-9, 20e-9)
    toneStackJCM800     = Components{Float64}(220e3, 1e6, 22e3, 33e3, 0.47e-9, 22e-9, 22e-9)
    toneStackRectifier  = Components{Float64}(250e3, 1e6, 25e3, 47e3, 0.50e-9, 20e-9, 20e-9)
    toneStackCustom     = Components{Float64}(250e3, 1e6, 25e3, 47e3, 0.50e-9, 20e-9, 20e-9)

    samplerate::Float64 = 48000.0

    nFreq = 2^14
    signal = [1; zeros(nFreq - 1)]'


    toneJCM = toneStackRational(toneStackJCM800, t, m, l, samplerate, Float64)
    toneSoldano = toneStackRational(toneStackSoldano, t, m, l, samplerate, Float64)
    toneEngl = toneStackRational(toneStackEnglSavage, t, m, l, samplerate, Float64)
    toneRectifier = toneStackRational(toneStackRectifier, t, m, l, samplerate, Float64)

    signal = zeros(nFreq)
    signal[1] = 1
    yJCM = filter!(signal, toneJCM)
    ySoldano = filter!(signal, toneSoldano)
    yEngl = filter!(signal, toneEngl)
    yRectifier = filter!(signal, toneRectifier)

    yFreqs = rfftfreq(size(yJCM)[1], samplerate) .+ 1.0

    yJCMSpectre = 20 * log10.((yJCM |> rfft .|> abs).^2)
    ySoldanoSpectre = 20 * log10.((ySoldano |> rfft .|> abs).^2)
    yEnglSpectre = 20 * log10.((yEngl |> rfft .|> abs).^2)
    yRectifierSpectre = 20 * log10.((yRectifier |> rfft .|> abs).^2)


    fig = Figure(size = (1200, 400))
    ax1 = Axis(fig[1, 1:2], xscale = log10)

    controlSliders = SliderGrid(fig[1, 3],
        (label = "trebble", range = 0:0.01:1, startvalue = 0.5, horizontal = true),
        (label = "middle", range = 0:0.01:1, startvalue = 0.5, horizontal = true),
        (label = "bass", range = 0:0.01:1, startvalue = 0.5, horizontal = true),

        (label = "R1", range = 200e3:10e3:300e3, startvalue = 250e3),
        (label = "R2", range = 500e3:100e3:2e6, startvalue = 1e6),
        (label = "R3", range = 10e3:1e3:30e3, startvalue = 20e3),
        (label = "R4", range = 30e3:1e3:50e3, startvalue = 47e3),

        (label = "C1", range = 0.4e-9:0.01e-9:0.8e-9, startvalue = 0.47e-9),
        (label = "C2", range = 15e-9:1e-9:50e-9, startvalue = 47e-9),
        (label = "C3", range = 15e-9:1e-9:50e-9, startvalue = 22e-9),
    )

    obs = [s.value for s in controlSliders.sliders]


    customCurve = lift(obs...) do slvalues...
        
        t, m, l = slvalues[1:3]
        components = slvalues[4:end]

        toneStackCustom = Components{Float64}(components...)

        toneFilter = toneStackRational(toneStackCustom, t, m, l, samplerate, Float64)
        
        y = filter!(signal, toneFilter)
        ySpectre = 20 * log10.((y |> rfft .|> abs).^2)

        [ySpectre...]
    end 

    otherStack = toneStackJCM800

    otherCurves = lift(obs[1:3]...) do slvalues...
        t, m, l = slvalues
        filter = toneStackRational(otherStack, t, m, l, samplerate, Float64)

        y = filter!(signal, filter)
        
        ySpectre = 20 * log10.((y |> rfft .|> abs).^2)
        
        [ySpectre...] 
    end

    lines!(ax1, yFreqs, customCurve, label = "custom curve")
    # lines!(ax1, yFreqs, otherCurves, label = "model curve")
    lines!(ax1, yFreqs, yJCMSpectre, label = "JCMSpectre")
    lines!(ax1, yFreqs, ySoldanoSpectre, label = "SoldanoSpectre")
    lines!(ax1, yFreqs, yEnglSpectre, label = "EnglSpectre")
    lines!(ax1, yFreqs, yRectifierSpectre, label = "RectifierSpectre")
    
    
    limits!(ax1, 20, 20000, -30, 0)
    axislegend(position = :rb)

    display(fig)

    return nothing
end 

DEBUG = false

DEBUG && @run main()
DEBUG || main()
