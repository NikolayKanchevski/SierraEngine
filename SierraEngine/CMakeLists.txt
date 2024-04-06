# === PROJECT SETTINGS === #
cmake_minimum_required(VERSION 3.14)
project(SierraEngine)

# === CONFIGURE SIERRA === #
set(SIERRA_ENABLE_LOGGING ON)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(SIERRA_BUILD_SHARED_LIBRARY ON)
else()
    set(SIERRA_BUILD_STATIC_LIBRARY ON)
endif()

# === ENABLE EXTENSIONS === #
set(SIERRA_ENABLE_IMGUI_EXTENSION ON)

set(CMAKE_CXX_STANDARD 20)
include(../Sierra/Sierra.cmake)

# === BUILD APPLICATION === #
SierraBuildApplication(src/main.cpp)

# === LINK SOURCE FILES === #
add_subdirectory(src)

# === LINK PRECOMPILED HEADERS === #
target_precompile_headers(SierraEngine PRIVATE src/srpch.h)

# === LINK LIBRARIES === #
add_subdirectory(vendor)