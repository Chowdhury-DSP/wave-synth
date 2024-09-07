cmake_minimum_required(VERSION 3.15)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.12" CACHE STRING "Minimum OS X deployment target")
if(WIN32)
    set(CMAKE_SYSTEM_VERSION 7.1 CACHE STRING INTERNAL FORCE) # Windows SDK for Windows 7 and up
endif()
project(Wave_Synth VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(COMPANY_NAME "chowdsp")
set(PRODUCT_NAME "Wave_Synth")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/modules/cmake")
add_subdirectory(modules)

# set default plugin formats to build
if(IOS)
    set(JUCE_FORMATS Standalone AUv3)
else()
    set(JUCE_FORMATS AU VST3 Standalone)
endif()

juce_add_plugin(Wave_Synth
    COMPANY_NAME "${COMPANY_NAME}"
    PLUGIN_MANUFACTURER_CODE Chow
    PLUGIN_CODE Ws24
    FORMATS ${JUCE_FORMATS}
    PRODUCT_NAME "${PRODUCT_NAME}"
    
    LV2URI https://github.com/Chowdhury-DSP/Wave_Synth
    
    MICROPHONE_PERMISSION_ENABLED TRUE
)

if(NOT IOS)
    clap_juce_extensions_plugin(
        TARGET Wave_Synth
        CLAP_ID "org.chowdsp.Wave_Synth"
        CLAP_FEATURES audio-effect
        CLAP_PROCESS_EVENTS_RESOLUTION_SAMPLES 64
        CLAP_USE_JUCE_PARAMETER_RANGES DISCRETE
    )
endif()

add_subdirectory(src)
include_directories(src)
add_subdirectory(res)

target_compile_definitions(Wave_Synth PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Wave_Synth PRIVATE juce_plugin_modules)
