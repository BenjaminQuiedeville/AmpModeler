import os
import sys

argc = len(sys.argv)

release_mode = False
config: str = ""

if argc > 1 and sys.argv[1] == "release":
    release_mode = True
    config = sys.argv[1]

elif argc > 1 and sys.argv[1] == "relwithdebug":
    release_mode = False
    config = sys.argv[1]

elif argc > 1 and sys.argv[1] == "debug":
    release_mode = False
    config = sys.argv[1]

else:
    release_mode = False
    config = "debug"


compile_flags = "/MP /std:c++20 /EHsc /nologo /LD /bigobj /MTd /W4 /Zi /Zc:wchar_t /Zc:forScope /Zc:inline"
link_flags = "/OPT:REF"

if release_mode:
    compile_flags += " /Ox /Ob2 /GL /Gy"
    link_flags += " /LTCG"

defines = " ".join([
    "/D _USE_MATH_DEFINES",

    "/D JUCE_MODAL_LOOPS_PERMITTED=1",
    "/D JUCE_GLOBAL_MODULE_SETTINGS_INCLUDED=1",
    "/D JUCE_MODULE_AVAILABLE_juce_core=1",
    "/D JUCE_MODULE_AVAILABLE_juce_audio_plugin_client=1",
    "/D JUCE_MODULE_AVAILABLE_juce_audio_processors=1",
    "/D JUCE_MODULE_AVAILABLE_juce_gui_extra=1",
    "/D JUCE_MODULE_AVAILABLE_juce_gui_basics=1",
    "/D JUCE_MODULE_AVAILABLE_juce_graphics=1",
    "/D JUCE_MODULE_AVAILABLE_juce_events=1",
    "/D JUCE_MODULE_AVAILABLE_juce_data_structures=1",
    "/D JUCE_MODULE_AVAILABLE_juce_audio_basics=1",
    "/D JUCE_MODULE_AVAILABLE_juce_audio_utils=1",
    "/D JUCE_MODULE_AVAILABLE_juce_audio_formats=1",
    "/D JUCE_MODULE_AVAILABLE_juce_audio_devices=1",
    
    "/D DEBUG=1",
    "/D _DEBUG=1",
    "/D _MT=1",
    
    "/D WIN32",
    "/D _WINDOWS",
    "/D JucePlugin_Build_VST3=1",
    "/D JucePlugin_IsSynth=0",
    
    "/D JucePlugin_ProducesMidiOutput=0",
    "/D JucePlugin_IsMidiEffect=0",
    "/D JucePlugin_WantsMidiInput=0",
    "/D JucePlugin_EditorRequiresKeyboardFocus=0",
    
    "/D JucePlugin_ManufacturerCode=0x48524d53",
    "/D \"JucePlugin_Manufacturer=\\\"Hermes140\\\"\"",
    "/D JucePlugin_PluginCode=0x414d504d",
    "/D JucePlugin_Version=0.2.0",
    "/D \"JucePlugin_Name=\\\"AmpModeler_{config}\\\"\"",
    "/D \"JucePlugin_Desc=\\\"AmpModeler_{config}\\\"\"",
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
    "/Ilibs/juce/modules/juce_audio_processors/format_types/VST3_SDK"
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
    "libs/juce/modules/juce_audio_plugin_client/juce_audio_plugin_client_VST3.cpp",  
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
])

libs = " ".join([
    "kernel32.lib", 
    "user32.lib", 
    "gdi32.lib", 
    "shell32.lib", 
    "ole32.lib", 
    "oleaut32.lib", 
    "uuid.lib", 
    "comdlg32.lib", 
])

command = f"cl {compile_flags} /Fe:AmpSimp_{config}.vst3 /Fd:AmpSimp{config}.pdb {defines} {includes} {plugin_sources} {juce_sources} /link {link_flags} {libs}"

#/Fd:AmpSimp.pdb
print(command)

return_code = os.system(command)

if return_code != 0:
    print("Problems during compilation, exiting")
    exit(-1)

print("copying binaries to \"C:/Program Files/Common Files/VST3/\"")
os.system(f"cp AmpSimp_{config}.vst3 \"C:/Program Files/Common Files/VST3/AmpSimp_{config}.vst3\"")
os.system("rm *.obj")
