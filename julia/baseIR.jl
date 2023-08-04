using WAV
using DelimitedFiles 

function main()

    filepath :: String = "01 Modern Standard.wav"

    (IR, _, _, _) = WAV.wavread(filepath)

    print(size(IR))
    writedlm("baseIR.txt", IR', ',')

end 
main()
