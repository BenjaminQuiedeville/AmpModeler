module Convolution
using Plots
using WAV
using FFTW
using DSP

#=
    overlap add blockbased convolution for speaker IR convolution

    https://thewolfsound.com/fast-convolution-fft-based-overlap-add-overlap-save-partitioned/

    suivre le tuto implémenter le buffer de stockages des inputsBlocks(tout est normalement puissance de 2 donc c'est bon)

    calculer la grosse FFT des inputs blocks 
    faire la multiplication dans le domaine des fréquences
    faire la IFFT et prendre les blockSize dernier samples vers la sortie


=#

mutable struct ConvolutionState

    buffer      :: Vector 
    bufferIndex :: UInt32
    length      :: UInt32

    ConvolutionState() = new()

end

mutable struct ImpulseResponse

    data       :: Vector
    length     :: Int
    dft        :: Vector{Complex}
    samplerate :: Float64

    ImpulseResponse() = new()
end 

nextPowerOfTwo(n :: Int) :: Int = 
    2^(floor(Int, log2(n - 1) + 1))

zeroPadding(vector :: Vector, size :: Int) = 
    vcat(vector, zeros(typeof(vector[1]), size - length(vector)))


function openImpulseResponse(filepath :: String) :: ImpulseResponse 

    ir = ImpulseResponse()

    (signal, samplerate, _, _) = wavread(filepath)
    signal = signal[:, 1]

    ir.samplerate = samplerate
    ir.length = nextPowerOfTwo(length(signal))
    ir.data = zeroPadding(signal, ir.length) 

    irFFT = fft(ir.data)
    println("length of the ir dft : $(length(irFFT))")

    ir.dft = irFFT

    return ir
end

function init(ir :: ImpulseResponse, state :: ConvolutionState) :: Nothing

    state.buffer = zeros(Float64, ir.length)
    state.bufferIndex = 0
    state.length = length(state.buffer)

    return nothing
end

function convolution(inputBlock :: Vector, ir :: ImpulseResponse, state :: ConvolutionState, samplerate :: Float64) :: Vector

    blockSize = length(inputBlock)

    for i in 0:(blockSize - 1) 
        writeIndex = state.bufferIndex + i
        writeIndex %= (state.length + 1)
        (writeIndex == 0) && (writeIndex += 1)

        state.buffer[writeIndex] = inputBlock[i + 1]
    end 

    fftInputBuffer = zeros(state.length)
    for i in 1:state.length
        writeIndex = (state.bufferIndex + i - 1) % (state.length + 1)
        (writeIndex == 0) && (writeIndex += 1)
        fftInputBuffer[i] = state.buffer[writeIndex]
    end
    state.bufferIndex += blockSize

    inputDFT = fft(fftInputBuffer)

    outputBuffer = (inputDFT .* ir.dft) |> ifft .|> real

    outputBlock = outputBuffer[(end - blockSize + 1):end]

    return outputBlock
end 


function main()

    impulseResponse = openImpulseResponse("./TestImpulseResponsesAuroraDSP/SinMix_Hesu412.wav")

    samplerate = impulseResponse.samplerate
    convState = ConvolutionState()

    signalLength = impulseResponse.length * 10
    blockSize :: Int = 128;
    # signal = randn(Float64, signalLength)

    T = LinRange(0, 10, Int(10 * samplerate))
    freqs = LinRange(1, 10000, length(T))
    # freq = 1000
    signal = sinpi.(2 * freqs .* T)

    outputSignal = zeros(length(signal))

    init(impulseResponse, convState)
    i :: Int = 1
    while i <= signalLength 

        if (i + blockSize - 1) > signalLength
            inputBlock = signal[i:end]
            inputBlock = zeroPadding(inputBlock, blockSize);
        
        else
            inputBlock = signal[i:(i + blockSize - 1)]
        end

        # outputBlock = inputBlock
        outputBlock = convolution(inputBlock, impulseResponse, convState, samplerate)


        if (i + blockSize - 1) > signalLength
            break
        else
            outputSignal[i:(i + blockSize - 1)] = outputBlock
        end

        i += blockSize
    end 

    pgram = welch_pgram(signal, fs = samplerate)

    p = plot()
    # plot!(signal[1:10000])
    plot!(pgram.freq, 20*log10.(pgram.power))
    # plot!(20*log10.(abs.(impulseResponse.dft[1:floor(Int,impulseResponse.length/2)])), xaxis = :log)
    display(p)

end

end # Convolution

import .Convolution as C; 
# @run C.main()
C.main()