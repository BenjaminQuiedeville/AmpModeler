using WAV
using DelimitedFiles 

function main()

    filepath :: String = "data/default_IR.wav"
    

    (IR, _, _, _) = WAV.wavread(filepath, format = "double")


    IR = Float32.(IR)
    @show size(IR)
    @show typeof(IR)
    @show IR[1]
    writedlm("experiments/baseIR.txt", IR', ',')

end 
main()
