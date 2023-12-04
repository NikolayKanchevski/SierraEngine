# === PROJECT SETTINGS === #
cmake_minimum_required(VERSION 3.14)
project(Sandbox)

# === INCLUDE SIERRA === #
include(../Sierra/Sierra.cmake)

# === SET ENGINE BUILD SETTINGS === #
set(CMAKE_CXX_STANDARD 20)
set(SIERRA_ENABLE_LOGGING ON)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(SIERRA_BUILD_STATIC_LIBRARY ON)
else()
    set(SIERRA_BUILD_SHARED_LIBRARY ON)
endif()

# === GENERATE EXECUTABLE === #
set(SOURCE_FILES src/main.cpp)
SierraBuildApplication(${SOURCE_FILES})