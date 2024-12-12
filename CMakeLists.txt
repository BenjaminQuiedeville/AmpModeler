cmake_minimum_required(VERSION 3.22)

project(AmpModeler VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 20)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(JUCE_SOURCE_DIR ${LIB_DIR}/juce)

add_subdirectory(${JUCE_SOURCE_DIR})

if (MSVC)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall -Wextra)
endif()

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME                Hermes140
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE 
    NEEDS_MIDI_OUTPUT           FALSE 
    PLUGIN_MANUFACTURER_CODE    HRMS 
    PLUGIN_CODE                 AMPM 
    FORMATS                     VST3 Standalone
    PRODUCT_NAME                "AmpModeler"
    
    COPY_PLUGIN_AFTER_BUILD     TRUE
)

target_sources(${PROJECT_NAME}
    PRIVATE 
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp 
        Source/IRLoader.cpp 
        Source/Preamp.cpp 
        Source/Tonestack.cpp
        Source/pffft/pffft.c
)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${JUCE_SOURCE_DIR}/modules
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC 
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_ASIO=0
        _USE_MATH_DEFINES=1
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE 
        juce::juce_audio_utils
        juce::juce_graphics
    PUBLIC 
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
