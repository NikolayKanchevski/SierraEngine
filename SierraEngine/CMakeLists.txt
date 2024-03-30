# === PROJECT SETTINGS === #
cmake_minimum_required(VERSION 3.14)
project(SierraEngine)

# === OPTIONS === #
option(SIERRA_ENGINE_BUILD_STANDALONE_APPLICATION "Whether to build final standalone binary, with the editor stripped out." OFF)

# === CONFIGURE SIERRA === #
set(SIERRA_ENABLE_LOGGING ON)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(SIERRA_BUILD_SHARED_LIBRARY ON)
else()
    set(SIERRA_BUILD_STATIC_LIBRARY ON)
endif()

set(CMAKE_CXX_STANDARD 20)
include(../Sierra/Sierra.cmake)

# === BUILD APPLICATION === #
SierraBuildApplication(src/main.cpp)

# === LINK SOURCE FILES === #
add_subdirectory(src)

# === ADD DEFINITIONS === #
if(SIERRA_ENGINE_BUILD_STANDALONE_APPLICATION)
    target_compile_definitions(SierraEngine PRIVATE "SR_STANDALONE_APPLICATION=1")
else()
    target_compile_definitions(SierraEngine PRIVATE "SR_STANDALONE_APPLICATION=0")
endif()

# === LINK PRECOMPILED HEADERS === #
target_precompile_headers(SierraEngine PRIVATE src/srpch.h)

# === LINK LIBRARIES === #
add_subdirectory(vendor)