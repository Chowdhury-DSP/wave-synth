include(SourceFileGroup)

setup_source_group(Wave_Synth PLUGIN_SRCS SOURCES
    Wave_Synth.cpp
)

file(GLOB_RECURSE juce_module_sources CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/JUCE/modules/juce_*/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/JUCE/modules/juce_*/*.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/chowdsp_utils/modules/*/chowdsp_*/*.cpp
)
set_source_files_properties(${juce_module_sources}
    TARGET_DIRECTORY Wave_Synth
    PROPERTIES SKIP_PRECOMPILE_HEADERS TRUE SKIP_UNITY_BUILD_INCLUSION TRUE
)
set_target_properties(Wave_Synth PROPERTIES
    UNITY_BUILD ON
    UNITY_BUILD_MODE GROUP
    UNITY_BUILD_BATCH_SIZE 8
)
target_precompile_headers(Wave_Synth PRIVATE pch.h)
