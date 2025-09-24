cmake_minimum_required(VERSION 3.22)

project(AmpModeler VERSION 0.3.0)

set(CMAKE_CXX_STANDARD 20)

set(USE_TRACY FALSE)

set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)
set(JUCE_SOURCE_DIR ${LIB_DIR}/juce)

# if(USE_TRACY)
#     set(TRACY_DIR W:/Tracy-Profiler/tracy-0.11.1)
#     # set(TRACY_DIR D:/Dev/tracy)
# endif()
 
add_subdirectory(${JUCE_SOURCE_DIR})

juce_add_plugin(${PROJECT_NAME}
    COMPANY_NAME                Hermes140
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE 
    NEEDS_MIDI_OUTPUT           FALSE 
    PLUGIN_MANUFACTURER_CODE    HRMS 
    PLUGIN_CODE                 AMPM 
    FORMATS                     VST3 Standalone
    PRODUCT_NAME                "AmpModeler"
    
    VST3_AUTO_MANIFEST          TRUE
    COPY_PLUGIN_AFTER_BUILD     TRUE
)

if (MSVC)
    add_compile_options(/W3 /arch:AVX2)
    
    set(OPTI_FLAGS "/Qpar /fp:fast")

    # target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:DEBUG>:SHELL:/MTd>")
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELEASE>:SHELL:${OPTI_FLAGS}>") # /MT
    target_compile_options(${PROJECT_NAME} PRIVATE "$<$<CONFIG:RELWITHDEBINFO>:SHELL:${OPTI_FLAGS}>") # /MTd
    
    # add_link_options(/NODEFAULTLIB:LIBCMTD)
else()
    add_compile_options(-Wall -Wextra -mavx2 -ffast-math -fno-math-errno)
    
    set(OPTI_FLAGS "-O3 ")
    set(DEBUG_FLAGS "-g -gcodeview")
    set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${DEBUG_FLAGS} ${OPTI_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE ${OPTI_FLAGS})
    
    add_link_options(-ffast-math -fno-math-errno -fuse-ld=lld)
endif()

target_sources(${PROJECT_NAME}
    PRIVATE 
        # Source/PluginProcessor.cpp
        # Source/PluginEditor.cpp 
        # Source/IRLoader.cpp 
        # Source/Preamp.cpp 
        Source/main.cpp
        libs/pffft/pffft.c
)

target_include_directories(${PROJECT_NAME}
    PRIVATE 
        ${LIB_DIR}
        ${JUCE_SOURCE_DIR}/modules
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC 
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_MODAL_LOOPS_PERMITTED=1
        JUCE_ASIO=0
        _USE_MATH_DEFINES
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

if(USE_TRACY)
    target_sources(${PROJECT_NAME} PUBLIC ${TRACY_DIR}/public/TracyClient.cpp)
    target_include_directories(${PROJECT_NAME} PUBLIC ${TRACY_DIR}/public)
    target_compile_definitions(${PROJECT_NAME} PUBLIC TRACY_ENABLE)    
    target_link_libraries(${PROJECT_NAME} PUBLIC Dbghelp.lib)
endif()
