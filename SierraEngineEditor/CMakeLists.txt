# === PROJECT SETTINGS === #
cmake_minimum_required(VERSION 3.14)
project(SierraEngineEditor)

# === CONFIGURE SIERRA ENGINE === #
set(SIERRA_ENGINE_ENABLE_LOGGING ON)
set(SIERRA_ENGINE_BUILD_SHARED_LIBRARY ON)

# === INCLUDE SIERRA ENGINE === #
set(CMAKE_CXX_STANDARD 20)
include(../SierraEngine/SierraEngine.cmake)

# === BUILD APPLICATION === #
SierraEngineBuildApplication(src/EntryPoint.cpp)

# === LINK SOURCE FILES === #
add_subdirectory(src)