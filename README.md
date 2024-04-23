# Amp Modeler

## Dependency to build the plugin :
- CMake
- A C++ compiler (mainly built with MSVC)

## Build the project : 
At the root of project do

cmake -S . -B build -G "Generator"

Generator can be either :
"Visual Studio 17 2022"
"Unix Makefiles"
"MSYS Makefiles"

to build :
    cmake --build build --config Release