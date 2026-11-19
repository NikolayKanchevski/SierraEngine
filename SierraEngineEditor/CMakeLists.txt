# === PROJECT SETTINGS === #
cmake_minimum_required(VERSION 3.22)
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

# === COPY RUNTIME LIBRARIES === #
# Windows only looks for libraries next to executables (other platforms find them through the executable's runtime search paths)
# NOTE: Post-build steps can only be attached to a target from the directory it was created in, which is this one
if(WIN32)
    add_custom_command(TARGET SierraEngineEditor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:slang> $<TARGET_FILE_DIR:SierraEngineEditor>)
endif()

# === SET FLAGS === #
set_property(TARGET SierraEngineEditor PROPERTY COMPILE_WARNING_AS_ERROR ON)