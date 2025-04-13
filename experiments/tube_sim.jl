
module TubeSim 
using GLMakie

mutable struct Params
    pre_bias :: Float64
    post_bias :: Float64
    pos_lin_range :: Float64
    grid_cond_thresh :: Float64
    ratio :: Float64
    knee_width :: Float64
    negative_clip_point :: Float64
    
    Params() = new()
end 

mutable struct Onepole
    
    b0 :: Float64
    a1 :: Float64
    y1 :: Float64
    
    Onepole() = new(1.0, 0.0, 0.0)
end 

function onepole_process(filter :: Onepole, sample :: Float64)
    output = sample * filter.b0 - filter.a1 * filter.y1
    filter.y1 = output
    return output
end


function process(params :: Params, in_signal :: Vector, cap :: Onepole, samplerate :: Float64)

    out_signal = zeros(Float64, size(in_signal))
    grid_signal = zeros(Float64, size(in_signal))

    DCBlocker = Onepole()

    DCBlocker.b0 = sin(pi / samplerate * 15.0)
    DCBlocker.a1 = DCBlocker.b0 - 1.0

    if params.pre_bias > params.pos_lin_range 
        params.post_bias = 1.0 - exp(params.pos_lin_range - params.pre_bias) + params.pos_lin_range
    else 
        params.post_bias = params.pre_bias
    end
    
    
    for index in 1:length(in_signal)
        # grid conduction
        sample = in_signal[index]

        # grid_sample = sample
        grid_sample = 0.0
        
        if 2*(sample - params.grid_cond_thresh) > params.knee_width
            grid_sample = params.grid_cond_thresh + (sample - params.grid_cond_thresh)/params.ratio
        elseif 2*abs(sample - params.grid_cond_thresh) <= params.knee_width
            grid_sample = sample + ((1.0/params.ratio - 1.0) * (sample - params.grid_cond_thresh + params.knee_width/2)^2)/(2*params.knee_width)
        end
        
        grid_sample = onepole_process(cap, grid_sample)
        grid_signal[index] = grid_sample
        
        sample = sample - grid_sample

        sample *= -1.0
        sample += params.pre_bias

        # clipping 
        if sample > params.pos_lin_range 
            sample = 1.0 - exp(params.pos_lin_range - sample) + params.pos_lin_range        
        elseif sample < 0.0
            # sample *= 2.0/(3.0 * params.negative_clip_point)
            # sample = sample < -1.0 ? -2.0/3.0 : sample - 1.0/3.0 * sample^3
            # sample *= params.negative_clip_point * 3.0/2.0
        end 

        sample -= params.post_bias
        sample *= -1.0

        sample = sample - onepole_process(DCBlocker, sample)

        out_signal[index] = sample
    end
    
    return (out_signal, grid_signal)
end

function main() :: Figure
    samplerate :: Float64 = 48000.0

    nSamples = Int(3.0 * samplerate)
    
    T = LinRange(0.0, 3.0, nSamples)

    signal = 50.0 .* sin.(2*pi * T * 80.0)
    # signal = zeros(Float64, 1000)

    # signal = 2 .* sinpi.(2 * 0:1/samplerate:pi * 100)
    # signal .*= LinRange(0, 1, length(signal))
    env = LinRange(1.0, 0.0, Int(0.5*samplerate + 0.5* samplerate)).^2

    signal .*= [env; env; env]

    # signal, samplerate, _, _ = wavread("data/Metallica.wav")
    
    cap = Onepole()    
    tau = 1.0 * 1000.0
    cap.b0 = sin(pi / samplerate * 1/tau);
    cap.a1 = cap.b0 - 1.0;
    
    params = Params()
    
    params.pre_bias = 0.2
    params.post_bias = 0.0
    params.pos_lin_range = 0.2
    params.grid_cond_thresh = 1.0
    params.ratio = 1.0
    params.knee_width = params.ratio/4
    params.negative_clip_point = 3.0
    
    # out_signal = signal
    out_signal, grid_signal = process(params, signal, cap, samplerate)

    figure = Figure(size = (1920, 1080))
    ax = Axis(figure[1, 1])
    # lines!(signal, label = "input signal")

    ax2 = Axis(figure[2, 1])

    lines!(ax, T, out_signal, label = "tube_output", color=:red)
    lines!(ax2, grid_signal, label = "grid signal", color=:orange)
    # lines!(signal, label = "tube_input", color=:blue)
    # axislegend(; position = :lb)

    # freq_axis = Axis(figure[2, 1], xscale = log10)
    # lines!(freq_axis, 20.0 * log10.(abs.(rfft(signal[1:8192]))))
    # ylims!(freq_axis, [0.0, 100.0])
    return figure
end 

end # module

import .TubeSim.main
# figure = TubeSim.main()
@run main()