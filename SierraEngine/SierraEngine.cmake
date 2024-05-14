# === CMAKE SETTINGS === #
cmake_minimum_required(VERSION 3.14)

# === OPTIONS === #
set(SIERRA_ENGINE_APPLICATION_NAME ${PROJECT_NAME} CACHE STRING "Name of the application (used for the binary and code signing).")
set(SIERRA_ENGINE_APPLICATION_ICON_ICO "../Media/SierraExecutableIcon.ico" CACHE FILEPATH "Absolute path to an .ico image to use as an icon for the generated binary on non-Apple desktop platforms.")
set(SIERRA_ENGINE_APPLICATION_ICON_ICNS "../Media/SierraExecutableIcon.icns" CACHE FILEPATH "Absolute path to an .icns image to use as an icon for the generated binary on Apple platforms.")
set(SIERRA_ENGINE_APPLICATION_ICON_PNG "../Media/SierraExecutableIcon.png" CACHE FILEPATH "Absolute path to an .png image to use as an icon for the generated binary on Linux platforms.")

option(SIERRA_ENGINE_BUILD_STATIC_LIBRARY "Whether to build the engine as a static library, which is embedded in the application." OFF)
option(SIERRA_ENGINE_BUILD_SHARED_LIBRARY "Whether to build the engine as a shared library, which is shipped as a separate file (.dll, .dylib, etc.)" OFF)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(SIERRA_ENGINE_ENABLE_LOGGING "Whether to enable Application and Engine logging. Do note this option is very likely to cause big memory leaks, due to bugs in graphics API drivers (not on Sierra's end)." ON)
    option(SIERRA_ENGINE_ENABLE_OPTIMIZATIONS "Whether to enable optimizations." OFF)
else()
    option(SIERRA_ENGINE_ENABLE_LOGGING "Whether to enable both Application and Engine logging. Do note this option is very likely to cause big memory leaks, due to bugs in graphics API drivers (not on Sierra's end)." OFF)
    option(SIERRA_ENGINE_ENABLE_OPTIMIZATIONS "Whether to enable optimizations." ON)
endif()

# === INCLUDE CORE LIBRARY === #
set(SIERRA_APPLICATION_NAME ${SIERRA_ENGINE_APPLICATION_NAME})
set(SIERRA_APPLICATION_ICON_ICO ${SIERRA_ENGINE_APPLICATION_ICON_ICO})
set(SIERRA_APPLICATION_ICON_ICNS ${SIERRA_ENGINE_APPLICATION_ICON_ICNS})
set(SIERRA_APPLICATION_ICON_PNG ${SIERRA_ENGINE_APPLICATION_ICON_PNG})

set(SIERRA_BUILD_STATIC_LIBRARY ON)
set(SIERRA_BUILD_DYNAMIC_LIBRARY OFF)

set(SIERRA_ENABLE_LOGGING ${SIERRA_ENGINE_ENABLE_LOGGING})
set(SIERRA_ENABLE_OPTIMIZATIONS ${SIERRA_ENGINE_ENABLE_OPTIMIZATIONS})

set(SIERRA_ENABLE_IMGUI_EXTENSION ON)
include(../Sierra/Sierra.cmake)

# === BUILD API LIBRARY === #
set(SIERRA_ENGINE_DIRECTORY_PATH ${CMAKE_CURRENT_LIST_DIR})
add_subdirectory(${SIERRA_ENGINE_DIRECTORY_PATH}/src/ ${SIERRA_ENGINE_DIRECTORY_PATH}/src/)

function(SierraEngineBuildApplication SOURCE_FILES)
    # Create executable
    SierraBuildApplication(${SOURCE_FILES})

    # Delete static library after build
    if(SIERRA_ENGINE_BUILD_STATIC_LIBRARY)
        add_custom_command(TARGET ${SIERRA_ENGINE_APPLICATION_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove $<TARGET_FILE:SierraEngine>)
    endif()

    # Link core library to engine
    target_link_libraries(SierraEngine PUBLIC Sierra)
    target_include_directories(SierraEngine PUBLIC ${SIERRA_DIRECTORY_PATH}/include/)

    # Link engine library to application
    target_link_libraries(${SIERRA_ENGINE_APPLICATION_NAME} PRIVATE SierraEngine)
    target_include_directories(${SIERRA_ENGINE_APPLICATION_NAME} PRIVATE ${SIERRA_ENGINE_DIRECTORY_PATH}/include/)

    # Run update scripts
    find_package(Python)
    if(Python_FOUND)
        add_custom_command(TARGET ${SIERRA_APPLICATION_NAME} POST_BUILD COMMAND ${Python_EXECUTABLE} ${SIERRA_ENGINE_DIRECTORY_PATH}/scripts/UpdateProject.py)
        message(STATUS "[Sierra Engine]: Running project update scripts...")
    else()
        message(WARNING "[Sierra Engine]: Python install was not found on the machine. Project update scripts cannot be run!")
    endif()
endfunction()