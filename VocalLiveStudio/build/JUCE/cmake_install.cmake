# Install script for directory: D:/auto-tool/JUCE

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/VocalLiveStudio")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "C:/Users/vohoa/AppData/Local/Microsoft/WinGet/Packages/BrechtSanders.WinLibs.MCF.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe/mingw64/bin/objdump.exe")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/auto-tool/VocalLiveStudio/build/JUCE/modules/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("D:/auto-tool/VocalLiveStudio/build/JUCE/extras/Build/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/JUCE-7.0.12" TYPE FILE FILES
    "D:/auto-tool/VocalLiveStudio/build/JUCE/JUCEConfigVersion.cmake"
    "D:/auto-tool/VocalLiveStudio/build/JUCE/JUCEConfig.cmake"
    "D:/auto-tool/JUCE/extras/Build/CMake/JUCECheckAtomic.cmake"
    "D:/auto-tool/JUCE/extras/Build/CMake/JUCEHelperTargets.cmake"
    "D:/auto-tool/JUCE/extras/Build/CMake/JUCEModuleSupport.cmake"
    "D:/auto-tool/JUCE/extras/Build/CMake/JUCEUtils.cmake"
    "D:/auto-tool/JUCE/extras/Build/CMake/JuceLV2Defines.h.in"
    "D:/auto-tool/JUCE/extras/Build/CMake/LaunchScreen.storyboard"
    "D:/auto-tool/JUCE/extras/Build/CMake/PIPAudioProcessor.cpp.in"
    "D:/auto-tool/JUCE/extras/Build/CMake/PIPAudioProcessorWithARA.cpp.in"
    "D:/auto-tool/JUCE/extras/Build/CMake/PIPComponent.cpp.in"
    "D:/auto-tool/JUCE/extras/Build/CMake/PIPConsole.cpp.in"
    "D:/auto-tool/JUCE/extras/Build/CMake/RecentFilesMenuTemplate.nib"
    "D:/auto-tool/JUCE/extras/Build/CMake/UnityPluginGUIScript.cs.in"
    "D:/auto-tool/JUCE/extras/Build/CMake/checkBundleSigning.cmake"
    "D:/auto-tool/JUCE/extras/Build/CMake/copyDir.cmake"
    "D:/auto-tool/JUCE/extras/Build/CMake/juce_runtime_arch_detection.cpp"
    "D:/auto-tool/JUCE/extras/Build/CMake/juce_LinuxSubprocessHelper.cpp"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "D:/auto-tool/VocalLiveStudio/build/JUCE/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
