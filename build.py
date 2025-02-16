import os
import os.path as path
import sys

argc = len(sys.argv)

def Print(message: str): print(message, file = sys.stdout, flush = True)

work_dir = os.getcwd()
release = False
debug = False
juce_only = False
use_tracy_profiler = False
tracy_dir = "W:/Tracy-Profiler/tracy-0.11.1"
config: str = ""

if argc == 1:
    Print("First call 'build.py juce' to build the juce dependencies and then call 'build.py debug/release/relwithdebug'")
    exit(0)
    
elif sys.argv[1] == "release":
    release = True
    debug = False
    config = sys.argv[1]

elif sys.argv[1] == "relwithdebug":
    release = True
    debug = True
    config = sys.argv[1]

elif sys.argv[1] == "debug":
    release = False
    debug = True
    config = sys.argv[1]
    
elif sys.argv[1] == "juce":
    juce_only = True
    debug = True
        
else: 
    Print("wrong config, use either 'release', 'relwithdebug', 'debug', 'juce'")
    exit(-1)

plugin_name = f"AmpSimp_{config}"
compile_flags = "/MP /std:c++20 /EHsc /nologo /LD /bigobj /MTd /W4 /Zc:wchar_t /Zc:forScope /Zc:inline"
optim_flags = " /Ox /Ob2 /GL /Gy /arch:AVX2"
debugging_flags = " /Zi"

link_flags = "/OPT:REF"
if release: link_flags += " /LTCG"
if debug: link_flags += " /DEBUG"

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
    "/D JucePlugin_PluginCode=0x41707370",
    "/D JucePlugin_Version=0.3.0",
    f"/D \"JucePlugin_Name=\\\"{plugin_name}\\\"\"",
    f"/D \"JucePlugin_Desc=\\\"{plugin_name}\\\"\"",
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
    "/Ilibs/juce/modules/juce_audio_processors/format_types/VST3_SDK",
    "/Ilibs",
])

plugin_sources = " ".join([
    # "Source/PluginProcessor.cpp",
    # "Source/PluginEditor.cpp",
    # "Source/Preamp.cpp",
    # "Source/IRLoader.cpp", 
    # "Source/Tonestack.cpp",
    "Source/main.cpp",
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
    "libs/pffft/pffft.c"
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

def build_juce(sources: str, out_path: str, flags: str) -> None:
    os.makedirs(out_path)

    command = f"cl {flags} /c /Fo:{out_path}/ /Fd:{out_path}/ {defines} {includes} {juce_sources}"
    Print(command)
    return_code = os.system(command)
    
    assert(not return_code)
    return    

if use_tracy_profiler:
    Print("Compiling with Tracy Profiler")
    plugin_sources += f" {tracy_dir}/public/TracyClient.cpp"
    includes += f" /I{tracy_dir}/public"
    defines += " /D TRACY_ENABLE"

juce_build_dir: str = f"juce_build/{config}"

command_flags = ""
if config == "debug":            command_flags = compile_flags + debugging_flags
elif config == "release":        command_flags = compile_flags + optim_flags
elif config == "relwithdebug":   command_flags = compile_flags + optim_flags + debugging_flags 

# see if we need to rebuild the juce dependencies
if (not path.isdir(juce_build_dir) and not juce_only):
    build_juce(juce_sources, juce_build_dir, command_flags)
    
elif juce_only:
    os.system("rm -r juce_build")    
    
    build_juce(juce_sources, "juce_build/debug",        compile_flags + debugging_flags)
    build_juce(juce_sources, "juce_build/release",      compile_flags + optim_flags)
    build_juce(juce_sources, "juce_build/relwithdebug", compile_flags + optim_flags + debugging_flags )
    exit(0)

juce_objects = " ".join([f"{juce_build_dir}/{file}" for file in filter(lambda string : ".obj" in string, os.listdir(juce_build_dir))])

command = f"cl {command_flags} /Fe:{plugin_name}.vst3 /Fd:{plugin_name}.pdb {defines} {includes} {plugin_sources} /link {link_flags} {juce_objects} {libs}"

# Print(command)
return_code = os.system(command)

if return_code != 0:
    Print("Problems during compilation, exiting")
    exit(-1)

Print("copying binaries to \"C:/Program Files/Common Files/VST3/\"")
os.system(f"cp {plugin_name}.vst3 \"C:/Program Files/Common Files/VST3/{plugin_name}.vst3\"")
os.system("rm *.obj")
