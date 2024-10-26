using GLMakie
using Statistics
using Chain
using WAV
using ProgressMeter

include("clipping_curves.jl")

mutable struct Integrator
    state :: Vector{Float64}
    state_index :: Int
    sum :: Float64
end 

mutable struct DCBlocker
    b0 :: Float64
    a1 :: Float64
    y1 :: Float64

    function DCBlocker(freq :: Float64)
        b0 = sin(pi * freq)
        a1 = b0 - 1.0
        y1 = 0.0

        return new(b0, a1, y1)
    end
end



function main() :: Figure
    samplerate :: Float64 = 48000.0

    signal = 0.2 .* sin.(0:0.01:4*pi)

    # signal = 2 .* sinpi.(2 * 0:1/samplerate:0.7 * 200)
    # signal .*= LinRange(0, 1, length(signal))

    # signal, samplerate, _, _ = wavread("data/Metallica.wav")

    # @show sum(signal)

    # sample based soft knee compression

    figure = Figure()
    ax = Axis(figure[1, 1])
    lines!(signal, label = "input signal")
    threshold = 1.0
    ratio = 4
    knee_width = 0.01
    for index in 1:length(signal)
        sample = signal[index]

        if 2*(sample - threshold) > knee_width
            sample = threshold + (sample - threshold)/ratio
        elseif 2*abs(sample - threshold) <= knee_width
            sample = sample + ((1/ratio - 1) * (sample - threshold + knee_width/2)^2)/(2*knee_width)
        end

        signal[index] = sample
    end

    lines!(signal, label = "grid_conduction")

    gain = 100

    for index in 1:length(signal)
        sample = -signal[index] * gain
        sample /= 10
        sample = asym_tanh(sample, -2.0, 0.0)
        sample *= 10

        signal[index] = sample
    end 

    lines!(-signal, label = "tube_output")

    axislegend(; position = :lb)

    return figure
end 
figure = main()