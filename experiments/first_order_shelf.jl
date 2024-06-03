using GLMakie
using FFTW


mutable struct FirstOrderShelf

    b0 :: Float64
    b1 :: Float64
    a1 :: Float64

    x1 :: Float64
    y1 :: Float64

    out_gain :: Float64

    FirstOrderShelf() = new(1, 0, 0, 0, 0, 1)
end 

dbtoa(gain :: Number) = 10^(gain/20)
atodb(gain :: Number) = 20 * log10(gain)

function set_coeffs!(f :: FirstOrderShelf, freq :: Number, gaindb :: Number, samplerate :: Number, type :: Symbol) :: Nothing

    gain_linear = 1.0

    if type == :lowshelf
        f.out_gain = dbtoa(gaindb)
        gain_linear = dbtoa(-gaindb)
    elseif type == :highshelf
        f.out_gain = 1.0
        gain_linear = dbtoa(gaindb)
    else
        @assert(false && "wrong type of filter")
    end

    freq_radian = freq / samplerate * π

    eta =(gain_linear + 1)/(gain_linear - 1)
    rho = sin(π * freq_radian * 0.5 - π/4) / sin(π * freq_radian * 0.5 + π/4)

    alpha1 = gain_linear == 1 ? 0 : eta - sign(eta)*sqrt(eta^2 -1)

    beta0 = ((1 + gain_linear) + (1 - gain_linear) * alpha1) * 0.5
    beta1 = ((1 - gain_linear) + (1 + gain_linear) * alpha1) * 0.5

    f.b0 = (beta0 + rho * beta1)/(1 + rho * alpha1)
    f.b1 = (beta1 + rho * beta0)/(1 + rho * alpha1)    
    f.a1 = (rho + alpha1)/(1 + rho * alpha1)

    return nothing
end

function process!(f :: FirstOrderShelf, signal :: Vector) :: Nothing

    for i in 1:length(signal)

        out_sample = f.b0 * signal[i] + f.b1 * f.x1 - f.a1 * f.y1

        f.x1 = signal[i]
        f.y1 = out_sample;
        signal[i] = out_sample * f.out_gain
    end
    return nothing
end 



function main() :: Nothing

    samplerate = 48000.0

    nfft = 4096
    signal = zeros(nfft)
    signal[1] = 1.0
    gain_db = -3.0
    freq = 1000.0

    filter = FirstOrderShelf()
    set_coeffs!(filter, freq, gain_db, samplerate, :lowshelf)
    process!(filter, signal)


    spectrum = signal |> rfft .|> abs .|> atodb
    freqs = rfftfreq(nfft, samplerate) .+ 0.01

    fig = Figure()
    ax = Axis(fig[1, 1], xscale = log10, xminorticksvisible = true, xminorgridvisible = true, xminorticks = IntervalsBetween(5))
    lines!(ax, freqs, spectrum)
    xlims!(20, 20000)
    display(fig)

    return nothing 
end 

main()
