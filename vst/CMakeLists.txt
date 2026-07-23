cmake_minimum_required(VERSION 3.22)
project(MelodicDelay VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find JUCE
find_package(JUCE REQUIRED)

# Core DSP library
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/core-build)

# JUCE Plugin
juce_add_plugin(MelodicDelay
    PRODUCT_NAME "Melodic Delay"
    COMPANY_NAME "NineLegsDog"
    PLUGIN_MANUFACTURER_CODE NLDl
    PLUGIN_CODE MDly
    FORMATS VST3 Standalone
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES "Fx" "Delay"
)

target_sources(MelodicDelay PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_include_directories(MelodicDelay PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/core
)

target_link_libraries(MelodicDelay
    PRIVATE
        melodic-delay-core
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
)

target_compile_definitions(MelodicDelay
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_FLAC=0
        JUCE_USE_OGGVORBIS=0
)
