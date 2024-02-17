# === CMAKE SETTINGS === #
cmake_minimum_required(VERSION 3.14)
set(CMAKE_SUPPRESS_REGENERATION true)

# === INCLUDED FILES === #
set(SIERRA_DIRECTORY ${CMAKE_CURRENT_LIST_DIR})
include(${SIERRA_DIRECTORY}/cmake/PlatformDetection.cmake)
include(${SIERRA_DIRECTORY}/cmake/CompilerDetection.cmake)

# === OPTIONS === $
set(SIERRA_APPLICATION_NAME ${PROJECT_NAME} CACHE STRING "Name of the application (used for the binary and code signing).")
set(SIERRA_APPLICATION_VERSION_MAJOR 1 CACHE STRING "Major version of the application.")
set(SIERRA_APPLICATION_VERSION_MINOR 0 CACHE STRING "Minor version of the application.")
set(SIERRA_APPLICATION_VERSION_PATCH 0 CACHE STRING "Patch number of the application.")
set(SIERRA_APPLICATION_ICON_ICO "../Media/SierraExecutableIcon.ico" CACHE FILEPATH "Absolute path to an .ico image to use as an icon for the generated binary on non-Apple desktop platforms.")
set(SIERRA_APPLICATION_ICON_ICNS "../Media/SierraExecutableIcon.icns" CACHE FILEPATH "Absolute path to an .icns image to use as an icon for the generated binary on Apple platforms.")
set(SIERRA_APPLICATION_ICON_PNG "../Media/SierraExecutableIcon.png" CACHE FILEPATH "Absolute path to an .icns image to use as an icon for the generated binary on Apple platforms.")

option(SIERRA_BUILD_STATIC_LIBRARY "Whether to build the engine as a static library, which is embedded in the application." OFF)
option(SIERRA_BUILD_SHARED_LIBRARY "Whether to build the engine as a shared library, which is shipped as a separate file (.dll, .dylib, etc.)" OFF)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(SIERRA_ENABLE_LOGGING "Whether to enable Application and Engine logging." ON)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Whether to enable optimizations." OFF)
else()
    option(SIERRA_ENABLE_LOGGING "Whether to enable both Application and Engine logging." OFF)
    option(SIERRA_ENABLE_OPTIMIZATIONS "Whether to enable optimizations." ON)
endif()

if(SIERRA_PLATFORM_WINDOWS OR SIERRA_PLATFORM_macOS OR SIERRA_PLATFORM_LINUX OR SIERRA_PLATFORM_ANDROID OR SIERRA_PLATFORM_iOS)
    option(SIERRA_BUILD_VULKAN "Whether to build Vulkan and its resources." ON)
endif()
if(SIERRA_PLATFORM_macOS OR SIERRA_PLATFORM_iOS)
     option(SIERRA_BUILD_METAL "Whether to build Metal and its resources." ON)
endif()

option(SIERRA_BUILD_IMGUI "Whether to build native ImGui support." ON)

# === CHECK IF REQUIREMENTS ARE MET === #
if(NOT CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 20)
    message(FATAL_ERROR "[Sierra]: Sierra requires C++ 20 to be explicitly selected before including Sierra.cmake!")
endif()
if(SIERRA_COMPILER_UNKNOWN)
    message(FATAL_ERROR "[Sierra]: Sierra cannot be built with unrecognized compiler [${CMAKE_CXX_COMPILER_ID}]!")
endif()
if(SIERRA_PLATFORM_UNKNOWN)
    message(FATAL_ERROR "[Sierra]: Sierra cannot be built for current platform [${CMAKE_SYSTEM_NAME}]!")
endif()

# === BUILD API LIBRARY === #
add_subdirectory(${SIERRA_DIRECTORY}/src ${SIERRA_DIRECTORY}/src)

function(SierraBuildApplication SOURCE_FILES)
    # === COMPILER SETTINGS === #
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

    # == OUTPUT GENERATION === #
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

    # Delete static library after build
    if(SIERRA_BUILD_STATIC_LIBRARY)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:Sierra>)
    endif()

    # === ENGINE LINKING === #
    target_link_libraries(${SIERRA_APPLICATION_NAME} PRIVATE Sierra)
    target_include_directories(${SIERRA_APPLICATION_NAME} PRIVATE ${SIERRA_DIRECTORY}/include/)

    # === RUN UPDATE SCRIPT === #
    find_package(Python)
    if(Python_FOUND)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} POST_BUILD COMMAND ${Python_EXECUTABLE} ${SIERRA_DIRECTORY}/scripts/UpdateProject.py)
        message(STATUS "[Sierra]: Running project update scripts...")
    else()
        message(WARNING "[Sierra]: Python install was not found on the machine. Project update scripts cannot be run!")
    endif()
endfunction()