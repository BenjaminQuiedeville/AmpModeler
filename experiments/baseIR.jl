using WAV
using DSP

filepath :: String = "data/IRs/Fat Classic.wav"

(IR, samplerate, _, _) = WAV.wavread(filepath, format = "double")
IR = IR[:]

IR .*= 1.0/maximum(IR)

@assert samplerate == 48000.0

IR_441 = DSP.Filters.resample(IR, 44100.0/samplerate)

@show length(IR)
@show length(IR_441)

open("Source/data/baseIRs.h", "w") do io 
    
    write(io, "#define BASE_IR_48_SIZE $(length(IR))\n")
    write(io, "static float baseIR_48[BASE_IR_48_SIZE] = {\n\t")
    
    for elem in IR
        write(io, "$(string(Float32(elem)))f, ")
    end
    write(io, "\n};\n\n")

    write(io, "#define BASE_IR_441_SIZE $(length(IR_441))\n")
    write(io, "static float baseIR_441[BASE_IR_441_SIZE] = {\n\t")
    
    for elem in IR_441
        write(io, "$(string(Float32(elem)))f, ")
    end
    write(io, "\n};\n")
end
