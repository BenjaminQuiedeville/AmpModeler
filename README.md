### Amp Modeler

# Todo :

IRs : free IRs distributed with FenrIR by Aurora DSP

écrire un algo d'upsampling custom

Corriger les courbes de clipping pour la distorsion

setup the project : at the root of project
    cmake -S . -B build -G "Generator"

Generator can be either :
"Visual Studio 17 2022"
"Unix Makefiles"
"MSYS Makefiles"

to build :
    cmake --build build --config Debug/Release
