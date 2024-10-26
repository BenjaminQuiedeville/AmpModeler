using FFTW
using WAV
using GLMakie

pad(vector :: Vector, target_length :: Integer)  = [vector; zeros(target_length - length(vector))]
spectrum(signal :: Vector) = 20 .* log10.(abs.(rfft(signal)))

function main() :: Nothing

    GLMakie.closeall()

    filename = "data/default_IR_48.wav"
    ir_signal, samplerate, _, _ = wavread(filename)
    ir_signal = ir_signal[:, 1]
    @show filename

    # filename = ""
    # samplerate = 48000.0
    # ir_signal_length = 2048
    # ir_signal = sinc.(LinRange(0, 5, ir_signal_length))

    block_size :: Int = 64
    fft_size :: Int = 2 * block_size
    dft_size = trunc(Int, fft_size/2 + 1)
    @assert dft_size == block_size+1
    
    @show length(ir_signal)
    @show samplerate
    @show block_size

    # prepare the impulse response blocks and transform them
    num_ir_blocks :: Int = round(length(ir_signal)/block_size, RoundUp)
    ir_dft_blocks = zeros(ComplexF64, dft_size, num_ir_blocks)
    @show num_ir_blocks
    ir_block = Float64[]
    for i in 0:num_ir_blocks-1

        start_index = i * block_size + 1

        if start_index + block_size > length(ir_signal)
            ir_block = ir_signal[start_index:end]
        else 
            ir_block = ir_signal[start_index:(start_index + block_size)]
        end
        
        ir_block_pad = pad(ir_block, fft_size)
        
        ir_block_dft = rfft(ir_block_pad)
        @assert length(ir_block_dft) == dft_size
        ir_dft_blocks[:, i + 1] = ir_block_dft
        
    end
    
    input_signal = zeros(Float64, block_size * 30)
    input_signal[1] = 1.0
    output_signal = zeros(size(input_signal))

    block = zeros(block_size)
    num_input_blocks = trunc(Int, length(input_signal) / block_size)

    freq_delay_line = zeros(ComplexF64, dft_size, num_ir_blocks)
    output_dft = zeros(ComplexF64, size(freq_delay_line[:, 1]))

    for block_index in 0:num_input_blocks-1

        # get the block
        time_buffer_index = block_index * block_size + 1
        block = input_signal[time_buffer_index:time_buffer_index + block_size - 1] 
        @assert(length(block) == block_size)
        padded_block = pad(block, fft_size)

        # shift the ir dft blocks
        for delay_line_index in num_ir_blocks:-1:2
            freq_delay_line[:, delay_line_index] .= freq_delay_line[:, delay_line_index-1]
        end

        freq_delay_line[:, 1] = rfft(padded_block)

        # accumulate the convolution
        output_dft[:] .= 0.0
        for index in 1:num_ir_blocks
            output_dft .+= freq_delay_line[:, index] .* ir_dft_blocks[:, index]
        end 

        output_block = irfft(output_dft[:, 1], fft_size)

        output_signal[time_buffer_index:time_buffer_index+block_size-1] .= output_block[1:block_size]

    end 

    fig = Figure(size = (1200, 600))
    ax_signal_time = Axis(fig[1, 1],                 title = "output signal, time domain")
    ax_signal_freq = Axis(fig[1, 2], xscale = log10, title = "output signal, freq domain")
    ax_ir_time     = Axis(fig[2, 1],                 title = "ir time domain")
    ax_ir_freq     = Axis(fig[2, 2], xscale = log10, title = "ir freq domain")

    @show output_signal[1:20]

    lines!(ax_signal_time, output_signal)
    
    lines!(ax_signal_freq, rfftfreq(length(output_signal), samplerate), spectrum(output_signal))
    xlims!(ax_signal_freq, [20, 20000])

    lines!(ax_ir_time, ir_signal)

    lines!(ax_ir_freq, rfftfreq(length(ir_signal), samplerate), spectrum(ir_signal))
    xlims!(ax_ir_freq, [20, 20000])

    display(fig)

    return nothing
end 

main()
