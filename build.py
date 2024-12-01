import os 

flags = "/MP /std:c++20 /EHsc /nologo /LD /bigobj /MTd /W4 /Ox /Ob2 /GL  /Zc:wchar_t /Zc:forScope /Zc:inline"

defines = " ".join([
    "/D _USE_MATH_DEFINES",

    "/D JUCE_MODAL_LOOPS_PERMITTED=1",
    "/D JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1",
    "/D DEBUG=1",
    "/D _DEBUG=1",
    "/D _MT=1",
    
    "/D WIN32",
    "/D _WINDOWS",
    "/D JucePlugin_Build_VST3=1",
    "/D JucePlugin_IsSynth=0",
    
    "/D JucePlugin_ManufacturerCode=0x48524d53",
    "/D \"JucePlugin_Manufacturer=\\\"Hermes140\\\"\"",
    "/D JucePlugin_PluginCode=0x414d504d",
    "/D JucePlugin_ProducesMidiOutput=0",
    "/D JucePlugin_IsMidiEffect=0",
    "/D JucePlugin_WantsMidiInput=0",
    "/D JucePlugin_EditorRequiresKeyboardFocus=0",
    "/D JucePlugin_Version=0.2.0",
    "/D \"JucePlugin_Name=\\\"AmpModeler\\\"\"",
    "/D \"JucePlugin_Desc=\\\"AmpModeler\\\"\"",
    "/D \"JucePlugin_VersionString=\\\"0.2.0\\\"\"",
    "/D \"JucePlugin_Vst3Category=\\\"Fx\\\"\"",
    "/D \"JucePlugin_ManufacturerWebsite=\\\"\\\"\"",
    "/D \"JucePlugin_ManufacturerEmail=\\\"\\\"\"",

    "/D JucePlugin_VersionCode=0x200",
    "/D JucePlugin_VSTUniqueID=JucePlugin_PluginCode",
    "/D JucePlugin_VSTCategory=kPlugCategEffect",
    
    "/D JUCE_VST3_CAN_REPLACE_VST2=0",
])

includes = " ".join([
    "/Ilibs/juce/modules",
    "/IC:/Users/benjamin/Dev/AmpModeler/libs/JUCE/modules/juce_audio_processors/format_types/VST3_SDK"
])

plugin_sources = " ".join([
    "Source/PluginProcessor.cpp",
    "Source/PluginEditor.cpp",
    "Source/Preamp.cpp",
    "Source/IRLoader.cpp", 
    "Source/Tonestack.cpp",
    "Source/pffft/pffft.c"
])

juce_sources = " ".join([
    "libs/juce/modules/juce_audio_basics/juce_audio_basics.cpp",
    "libs/juce/modules/juce_audio_devices/juce_audio_devices.cpp",
    "libs/juce/modules/juce_audio_formats/juce_audio_formats.cpp",
    "libs/juce/modules/juce_audio_processors/juce_audio_processors.cpp",
    "libs/juce/modules/juce_audio_utils/juce_audio_utils.cpp",
    "libs/juce/modules/juce_core/juce_core.cpp",
    "libs/juce/modules/juce_data_structures/juce_data_structures.cpp",
    "libs/juce/modules/juce_graphics/juce_graphics.cpp",
    "libs/juce/modules/juce_gui_basics/juce_gui_basics.cpp",
    "libs/juce/modules/juce_gui_extra/juce_gui_extra.cpp",
    "libs/juce/modules/juce_events/juce_events.cpp",
    "libs/juce/modules/juce_audio_plugin_client/juce_audio_plugin_client_VST3.cpp",  
])

libs = " ".join([
    "kernel32.lib", 
    "user32.lib", 
    "gdi32.lib", 
    "winspool.lib", 
    "shell32.lib", 
    "ole32.lib", 
    "oleaut32.lib", 
    "uuid.lib", 
    "comdlg32.lib", 
    "advapi32.lib",

])

command = f"cl {flags} /Fe:AmpSimp_release.vst3 {defines} {includes} {plugin_sources} {juce_sources} /link {libs}"

#/Fd:AmpSimp.pdb
print(command)

return_code = os.system(command)

