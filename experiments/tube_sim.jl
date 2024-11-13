using GLMakie
using WAV
using ProgressMeter
using FFTW
using Statistics

include("clipping_curves.jl")

function main() :: Figure
    samplerate :: Float64 = 48000.0

    signal = 1.0 .* sin.(LinRange(0.0, 15*pi, 8192))
    # signal = zeros(Float64, 1000)

    # signal = 2 .* sinpi.(2 * 0:1/samplerate:pi * 100)
    # signal .*= LinRange(0, 1, length(signal))

    # signal, samplerate, _, _ = wavread("data/Metallica.wav")

    # @show sum(signal)

    # sample based soft knee compression

    figure = Figure()
    ax = Axis(figure[1, 1])
    # lines!(signal * 100, label = "input signal")

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

    # lines!(signal, label = "grid_conduction")

    gain = 10^(35/20)
    bias = 2.0
    for index in 1:length(signal)
        sample = -signal[index]
        sample = asym_tanh(sample + bias, -2.0, 0.0)
        sample *= gain

        signal[index] = sample
    end 

    signal .-= mean(signal)

    lines!(signal, label = "tube_output", color=:red)

    axislegend(; position = :lb)

    freq_axis = Axis(figure[2, 1], xscale = log10)
    lines!(freq_axis, 20.0 * log10.(abs.(rfft(signal[1:8192]))))
    ylims!(freq_axis, [0.0, 100.0])
    return figure
end 
figure = main()