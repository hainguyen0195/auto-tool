cmake_minimum_required(VERSION 3.22)

project(VocalLiveStudio VERSION 1.0.0 LANGUAGES C CXX RC)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# JUCE integration
if (NOT TARGET juce::juce_core)
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../JUCE/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../JUCE" JUCE EXCLUDE_FROM_ALL)
    elseif (EXISTS "D:/auto-tool/JUCE/CMakeLists.txt")
        add_subdirectory("D:/auto-tool/JUCE" JUCE EXCLUDE_FROM_ALL)
    else()
        find_package(JUCE CONFIG REQUIRED)
    endif()
endif()

juce_add_gui_app(VocalLiveStudio
    PRODUCT_NAME "Vocal Live Studio"
    VERSION "1.0.0"
    BUNDLE_ID "com.vocallivestudio.app"
    COMPANY_NAME "Vocal Live Studio"
)

set_target_properties(VocalLiveStudio PROPERTIES
    OUTPUT_NAME "VocalLiveStudio"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

target_sources(VocalLiveStudio PRIVATE
    Source/Main.cpp
    Source/MainComponent.h
    Source/MainComponent.cpp
    Source/AudioEngine.h
    Source/AudioEngine.cpp
    Source/PitchDetector.h
    Source/PitchDetector.cpp
    Source/PitchShifter.h
    Source/PitchShifter.cpp
    Source/AutoTuneProcessor.h
    Source/AutoTuneProcessor.cpp
    Source/NoteMapper.h
    Source/NoteMapper.cpp
    Source/VocalEffects.h
    Source/VocalEffects.cpp
    Source/KaraokePlayer.h
    Source/KaraokePlayer.cpp
    Source/PresetManager.h
    Source/PresetManager.cpp
    Resources/app_icon.rc
)

target_compile_definitions(VocalLiveStudio PRIVATE
    _WIN32_WINNT=0x0601
    WINVER=0x0601
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_ASIO=0
    JUCE_WASAPI=1
    JUCE_DIRECTSOUND=1
)

target_link_libraries(VocalLiveStudio PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

target_link_options(VocalLiveStudio PRIVATE
    -static
    -static-libgcc
    -static-libstdc++
)

# Test target
add_executable(VocalLiveStudio_Tests
    Source/Tests.cpp
    Source/PitchDetector.h
    Source/PitchDetector.cpp
    Source/PitchShifter.h
    Source/PitchShifter.cpp
    Source/NoteMapper.h
    Source/NoteMapper.cpp
    Source/AutoTuneProcessor.h
    Source/AutoTuneProcessor.cpp
    Source/VocalEffects.h
    Source/VocalEffects.cpp
    Source/PresetManager.h
    Source/PresetManager.cpp
)

set_target_properties(VocalLiveStudio_Tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

target_compile_definitions(VocalLiveStudio_Tests PRIVATE
    _WIN32_WINNT=0x0601
    WINVER=0x0601
)

target_link_libraries(VocalLiveStudio_Tests PRIVATE
    juce::juce_audio_basics
    juce::juce_core
    juce::juce_data_structures
)

target_link_options(VocalLiveStudio_Tests PRIVATE
    -static
    -static-libgcc
    -static-libstdc++
)
