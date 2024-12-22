using GLMakie
using DSP
using DelimitedFiles


function main() 

    UPOLS_output = readdlm("UPOLS_output.txt");
    
    fftSize = 8192
    pgram = welch_pgram(UPOLS_output[:], fftSize, div(fftSize,2); fs = 44100.0)
   
    fig = Figure()
    ax = Axis(fig[1, 1], xscale = log10)
    lines!(ax, pgram.freq, 20.0 * log10.(pgram.power))
    xlims!(ax, 20.0, 20000.0)
    display(fig)    
    
    return nothing
end







