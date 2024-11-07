using GLMakie
using Statistics
using Chain
using WAV
using ProgressMeter

include("clipping_curves.jl")

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

    gain = 10^(35/20)

    for index in 1:length(signal)
        sample = -signal[index]
        sample = asym_tanh(sample, -2.0, 0.0)
        sample *= gain

        signal[index] = sample
    end 

    lines!(-signal, label = "tube_output", color=:red)

    axislegend(; position = :lb)

    return figure
end 
figure = main()