# === CMAKE SETTINGS === #
cmake_minimum_required(VERSION 3.14)
set(CMAKE_SUPPRESS_REGENERATION true)

# === INCLUDED FILES === #
set(SIERRA_DIRECTORY_PATH ${CMAKE_CURRENT_LIST_DIR})
include(${SIERRA_DIRECTORY_PATH}/cmake/PlatformDetection.cmake)
include(${SIERRA_DIRECTORY_PATH}/cmake/CompilerDetection.cmake)

# === OPTIONS === #
set(SIERRA_APPLICATION_NAME ${PROJECT_NAME} CACHE STRING "Name of the application (used for the binary and code signing).")
set(SIERRA_APPLICATION_VERSION_MAJOR 1 CACHE STRING "Major version of the application.")
set(SIERRA_APPLICATION_VERSION_MINOR 0 CACHE STRING "Minor version of the application.")
set(SIERRA_APPLICATION_VERSION_PATCH 0 CACHE STRING "Patch number of the application.")
set(SIERRA_APPLICATION_ICON_ICO "../Media/SierraExecutableIcon.ico" CACHE FILEPATH "Absolute path to an .ico image to use as an icon for the generated binary on non-Apple desktop platforms.")
set(SIERRA_APPLICATION_ICON_ICNS "../Media/SierraExecutableIcon.icns" CACHE FILEPATH "Absolute path to an .icns image to use as an icon for the generated binary on Apple platforms.")
set(SIERRA_APPLICATION_ICON_PNG "../Media/SierraExecutableIcon.png" CACHE FILEPATH "Absolute path to an .png image to use as an icon for the generated binary on Linux platforms.")

option(SIERRA_BUILD_STATIC_LIBRARY "Whether to build the engine as a static library, which is embedded in the application." OFF)
option(SIERRA_BUILD_SHARED_LIBRARY "Whether to build the engine as a shared library, which is shipped as a separate file (.dll, .dylib, etc.)" OFF)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(SIERRA_ENABLE_LOGGING "Whether to enable Application and Engine logging. Do note this option is very likely to cause big memory leaks, due to bugs in graphics API drivers (not on Sierra's end)." ON)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Whether to enable optimizations." OFF)
else()
    option(SIERRA_ENABLE_LOGGING "Whether to enable both Application and Engine logging. Do note this option is very likely to cause big memory leaks, due to bugs in graphics API drivers (not on Sierra's end)." OFF)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Whether to enable optimizations." ON)
endif()

if(SIERRA_PLATFORM_WINDOWS OR SIERRA_PLATFORM_macOS OR SIERRA_PLATFORM_LINUX OR SIERRA_PLATFORM_ANDROID OR SIERRA_PLATFORM_iOS)
    option(SIERRA_BUILD_VULKAN "Whether to build Vulkan and its resources." ON)
endif()
if(SIERRA_PLATFORM_macOS OR SIERRA_PLATFORM_iOS)
     option(SIERRA_BUILD_METAL "Whether to build Metal and its resources." ON)
endif()

option(SIERRA_ENABLE_IMGUI_EXTENSION "Whether to build the ImGui layer (ImGui would still need to be manually linked to application). Requires SIERRA_IMGUI_INCLUDE_DIRECTORY_PATH to be set." OFF)

# Validate usage
if(NOT CMAKE_CXX_STANDARD OR NOT CMAKE_CXX_STANDARD EQUAL 20)
    message(FATAL_ERROR "[Sierra]: Sierra runs under C++20 only, and needs the [CMAKE_CXX_STANDARD] version to be explicitly set to [20] prior to including Sierra.cmake!")
endif()
if(SIERRA_COMPILER_UNKNOWN)
    message(FATAL_ERROR "[Sierra]: Sierra cannot be built with unrecognized compiler [${CMAKE_CXX_COMPILER_ID}]!")
endif()
if(SIERRA_PLATFORM_UNKNOWN)
    message(FATAL_ERROR "[Sierra]: Sierra cannot be built for current platform [${CMAKE_SYSTEM_NAME}]!")
endif()
if((SIERRA_BUILD_STATIC_LIBRARY AND SIERRA_BUILD_SHARED_LIBRARY) OR (NOT SIERRA_BUILD_STATIC_LIBRARY AND NOT SIERRA_BUILD_SHARED_LIBRARY))
    message(FATAL_ERROR "[Sierra]: Illegal build configuration! You must compile with either SIERRA_BUILD_STATIC_LIBRARY or SIERRA_BUILD_SHARED_LIBRARY set, but not both!")
endif()

# Generate library
if(SIERRA_BUILD_STATIC_LIBRARY)
    message(STATUS "[Sierra]: Building Sierra as static library...")
    add_library(Sierra STATIC)
elseif(SIERRA_BUILD_SHARED_LIBRARY)
    message(STATUS "[Sierra]: Building Sierra as dynamic library...")
    add_library(Sierra SHARED)
endif()

# Determine library output path
set(SIERRA_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/")
if(SIERRA_PLATFORM_macOS)
    set(SIERRA_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${SIERRA_APPLICATION_NAME}.app/Contents/Frameworks/")
elseif(SIERRA_PLATFORM_iOS)
    set(SIERRA_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin/")
endif()

# Assign library output path
set_target_properties(Sierra PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${SIERRA_LIBRARY_OUTPUT_PATH}
    LIBRARY_OUTPUT_DIRECTORY ${SIERRA_LIBRARY_OUTPUT_PATH}
    RUNTIME_OUTPUT_DIRECTORY ${SIERRA_LIBRARY_OUTPUT_PATH}
)

# Set compiler options
if(SIERRA_ENABLE_OPTIMIZATIONS)
    if(SIERRA_COMPILER_MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Ox")
    elseif(SIERRA_COMPILER_CLANG OR SIERRA_COMPILER_GCC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    endif()
endif()

if(SIERRA_COMPILER_MSVC)
    add_compile_options(/wd4250)
    add_compile_options(/wd4251)
endif()

if(SIERRA_BUILD_ANDROID_STUDIO_PROJECT)
    BuildAndroidStudioProject()
    return()
endif()

function(SierraBuildApplication SOURCE_FILES)
    # Generate executable
    if(SIERRA_PLATFORM_WINDOWS)
        BuildWindowsExecutable(${SOURCE_FILES})
    elseif(SIERRA_PLATFORM_LINUX)
        BuildLinuxExecutable(${SOURCE_FILES})
    elseif(SIERRA_PLATFORM_macOS)
        BuildMacOSExecutable(${SOURCE_FILES})
    elseif(SIERRA_PLATFORM_ANDROID)
        BuildAndroidApplication(${SOURCE_FILES})
    elseif(SIERRA_PLATFORM_iOS)
        BuildIOSApplication(${SOURCE_FILES})
    endif()

    # Build library
    add_subdirectory(${SIERRA_DIRECTORY_PATH}/src/ ${SIERRA_DIRECTORY_PATH}/src/)

    # Delete static library after build
    if(SIERRA_BUILD_STATIC_LIBRARY)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:Sierra>)
    endif()
endfunction()