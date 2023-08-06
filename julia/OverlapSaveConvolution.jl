module Convolution
using Plots
using WAV
using FFTW

#=
    overlap add blockbased convolution for speaker IR convolution

    https://thewolfsound.com/fast-convolution-fft-based-overlap-add-overlap-save-partitioned/

    suivre le tuto implémenter le buffer de stockages des inputsBlocks(tout est normalement puissance de 2 donc c'est bon)

    calculer la grosse FFT des inputs blocks 
    faire la multiplication dans le domaine des fréquences
    faire la IFFT et prendre les blockSize dernier samples vers la sortie


=#

mutable struct ConvolutionState

    inputBlocks      :: Vector 
    inputBlocksIndex :: UInt32

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
    2^(log2(n - 1) + 1)

zeroPadding(vector :: Vector, size :: Int) = 
    vcat(vector, zeros(typeof(vector[1]), size - length(vector)))


function openImpulseResponse(filepath :: String) :: ImpulseResponse 

    ir = ImpulseResponse()

    (signal, samplerate, _, _) = wavread(filepath)

    ir.samplerate = samplerate
    ir.length = nextPowerOfTwo(length(signal))
    ir.data = zerosPadding(signal, ir.length) 

    irFFT = fft(signal)
    println("length of the ir dft : $(length(irFFT))")

    ir.dft = irFFT

    return ir
end

function init(ir :: ImpulseResponse, state :: ConvolutionState) :: Nothing

    state.inputBlocks = zeros(Float64, ir.length)
    state.inputBlocksIndex = 0

end

function convolution(inputBlock :: Vector, ir :: ImpulseResponse, state :: ConvolutionState, samplerate :: Float64) :: Vector

    blockSize = length(inputBlock)

end 


function main()

    impulseResponse = openImpulseResponse("../TestImpulseResponsesAuroraDSP/SinMix_Hesu412.wav")

    convState = ConvolutionState()

    signalLength = impulseResponse.length
    blockSize :: Int = 128;
    signal = rand(Float64, signalLength)

    outputSignal = zeros(length(signal))

    init(ir, state)

    for i in 1:signalLength 

        if i + blockSize > signalLength
            inputBlock = signal[i:end]
            inputBlock = zero_padding(inputBlock, blockSize);
        
        else
            inputBlock = signal[i:(i + blockSize)]
        end

        outputBlock = convolution(inputBlock, impulseResponse, convState, samplerate)

    end 

end

end # Convolution