module Convolution 
# file for overlap add convolution algorithm

using Plots
using FFTW
using WAV

zeroPad(signal, newLength) = vcat(signal, zeros(newLength - length(signal)))

mul(y :: Number) = x -> x*y 

spectrum(signal) = signal |> rfft .|> abs .|> log10 |> mul(20)

mutable struct ConvolutionState 
    buffer :: Vector
    bufferIndex :: Int
    fftLength :: Int
    convolutionLength :: Int

    ConvolutionState() = new()
end 

mutable struct ImpulseResponse 
    ir :: Vector
    dftir :: Vector

    ImpulseResponse() = new()
end 

function init(ir :: ImpulseResponse, state :: ConvolutionState, 
              filepath :: String, blockSize :: Integer)

    (impulseSignal, samplerate, _, _) = wavread(filepath)
    impulseSignal = impulseSignal[:, 1]
    convolutionLength = length(impulseSignal) + blockSize - 1

    fftLength :: Int = nextpow(2, convolutionLength)

    impulseSignal = zeroPad(impulseSignal, fftLength)
    impulseSignalDFT = fft(impulseSignal)

    ir.ir = impulseSignal
    ir.dftir = impulseSignalDFT

    state.fftLength = fftLength
    state.convolutionLength = convolutionLength
    state.buffer = zeros(2 * convolutionLength)
    state.bufferIndex = 1

    return nothing
end

function directConvolution(signal :: Vector, ir :: ImpulseResponse)

    outputSignal = zeros(length(signal) + length(ir.ir) - 1)

    for i in 1:length(signal)
        outputSignal[i:(i+length(ir.ir) - 1)] = signal[i] * ir.ir
    end

    return outputSignal
end

function process(inputBlock :: Vector, state :: ConvolutionState, ir :: ImpulseResponse) :: Vector

    stateBufferLength = length(state.buffer)

    inputDFT = fft(zeroPad(inputBlock, state.fftLength))
    output = (ir.dftir .* inputDFT) |> ifft |> real
    outputBlock = zeros(length(inputBlock))

    for i in 1:length(inputBlock)
        # clear the previous samples non need now to avoid overlaping
        index = state.bufferIndex - i 
        (index <= 0) && (index += stateBufferLength)
        (index > stateBufferLength) && (index = index % stateBufferLength + 1)
        state.buffer[index] = 0.0
    end 

    for i in 1:state.convolutionLength

        index = state.bufferIndex + i - 1
        (index > stateBufferLength) && (index = index % stateBufferLength + 1)
        (index <= 0) && (index = 1)
        
        state.buffer[index] += output[i]
    end
    
    for i in 1:length(inputBlock)
        index = state.bufferIndex + i - 1
        (index > stateBufferLength) && (index = index % stateBufferLength + 1)
        (index <= 0) && (index = 1)
        
        outputBlock[i] = state.buffer[index]
    end
    
    state.bufferIndex += length(inputBlock)
    state.bufferIndex %= stateBufferLength
    state.bufferIndex += 1

    return outputBlock
end 

function main() :: Nothing 
    run(`clear`)
    closeall()

    samplerate :: Float64 = 48000
    blockSize :: Int = 256
    signalDuration = 20 * blockSize
    signal = zeros(Float64, signalDuration)
    signal[1] = 1

    # signal[3001:3010] = ones(10)
    # signal = rand(signalDuration)

    convState = ConvolutionState()
    ir = ImpulseResponse()
    filePath = "../TestImpulseResponsesAuroraDSP/SinMix_BognerUbercab.wav"
    init(ir, convState, filePath, blockSize)

    outputSignal = zeros(length(signal))

    for inputIndex in 1:blockSize:(signalDuration)

        inputBlock = signal[inputIndex:(inputIndex + blockSize - 1)]
        outputBlock = process(inputBlock, convState, ir)
        outputSignal[inputIndex:(inputIndex + blockSize - 1)] = outputBlock

    end

    # outputSignal = directConvolution(signal, ir)

    p1 = begin
        PLOT_WIDTH::Int = 650
        gr(size = (PLOT_WIDTH, PLOT_WIDTH * 0.7))
        plot()
        # plot!(outputSignal)
        # plot!(signal)
        # plot!(ir.ir)
        plot!(spectrum(outputSignal))
        plot!(spectrum(ir.ir))
        plot!(xaxis = :log)

    end 
    display(p1)

    return nothing 
end 

end # Convolution
import .Convolution as C


@run C.main()
C.main()
