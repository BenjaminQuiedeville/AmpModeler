using WAV
using DelimitedFiles 

function main()

    filepath :: String = "data/default_IR_441.wav"
    

    (IR, _, _, _) = WAV.wavread(filepath, format = "double")

    IR = Float32.(IR)
    @show size(IR)
    @show typeof(IR)
    @show IR[1]
    writedlm("experiments/baseIR_441.txt", IR', "f, ")


    filepath = "data/default_IR_48.wav"
    
    (IR, _, _, _) = WAV.wavread(filepath, format = "double")

    IR = Float32.(IR)
    @show size(IR)
    @show typeof(IR)
    @show IR[1]
    writedlm("experiments/baseIR_48.txt", IR', "f, ")


end 
main()
