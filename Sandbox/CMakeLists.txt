# === PROJECT SETTINGS === #
cmake_minimum_required(VERSION 3.14)
project(Sandbox)

# === INCLUDE SIERRA === #
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(SIERRA_BUILD_STATIC_LIBRARY ON)
else()
    set(SIERRA_BUILD_SHARED_LIBRARY ON)
endif()

set(CMAKE_CXX_STANDARD 20)
include(../Sierra/Sierra.cmake)

# === BUILD EXAMPLES === #
macro(BuildExample EXAMPLE_NAME)
    set(SIERRA_APPLICATION_NAME ${EXAMPLE_NAME})
    SierraBuildApplication("examples/${EXAMPLE_NAME}.cpp")
endmacro()

BuildExample(Windowing)
BuildExample(HelloTriangle)
BuildExample(HelloCube)