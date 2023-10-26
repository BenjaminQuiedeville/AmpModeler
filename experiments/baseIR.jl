using WAV
using DelimitedFiles 

function main()

    filepath :: String = "SinMix_Hesu412.wav"

    (IR, _, _, _) = WAV.wavread(filepath, format = "single")


    IR = Float32.(IR)
    @show size(IR)
    @show typeof(IR)
    writedlm("baseIR.txt", IR', ',')

end 
main()
