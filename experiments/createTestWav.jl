# Creates one channel test wav files in 16, 24 and 32 bits;
# Test material for wavParse.cpp

using WAV
using Plots


const samplerate :: Float64 = 48000.0;
const signalSize :: UInt = 1000;

function main() :: Nothing

    signal = LinRange(0.5, 0.0, signalSize) |> collect;

    wavwrite(signal, "experiments/testWav16bits.wav"; 
             Fs=samplerate, nbits=16, compression=WAVE_FORMAT_PCM)
    wavwrite(signal, "experiments/testWav24bits.wav"; 
             Fs=samplerate, nbits=24, compression=WAVE_FORMAT_PCM)
    wavwrite(signal, "experiments/testWav32bits.wav"; 
             Fs=samplerate, nbits=32, compression=WAVE_FORMAT_PCM)
    return nothing
end 

main()