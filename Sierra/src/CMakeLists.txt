# Generate library
if(SIERRA_BUILD_STATIC_LIBRARY)
    message(STATUS "[Sierra]: Building Sierra as static library...")
    add_library(Sierra STATIC)
elseif(SIERRA_BUILD_SHARED_LIBRARY)
    message(STATUS "[Sierra]: Building Sierra as dynamic library...")
    add_library(Sierra SHARED)
else()
    message(FATAL_ERROR "[Sierra]: Incorrect configuration for Sierra - either SIERRA_BUILD_STATIC_LIBRARY, or SIERRA_BUILD_SHARED_LIBRARY must be turned on!")
endif()

# Set library output path
if(NOT SIERRA_PLATFORM_APPLE)
    set_target_properties(Sierra PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
    set_target_properties(Sierra PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/")
elseif(SIERRA_PLATFORM_macOS)
    set_target_properties(Sierra PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SIERRA_APPLICATION_NAME}.app/Contents/Frameworks/")
elseif(SIERRA_BUILD_XCODE_PROJECT)
    set_target_properties(Sierra PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
endif()

# Link engine modules
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Events)
add_subdirectory(Rendering)

# Link engine entry point
target_sources(Sierra PRIVATE
    srpch.h
    Application.h
    Application.cpp
    ../include/Sierra.h
)

# Export engine module symbols
if(SIERRA_BUILD_SHARED_LIBRARY)
    target_compile_definitions(Sierra PRIVATE "SR_BUILD_SHARED_LIBRARY")
    target_compile_definitions(Sierra PUBLIC "SR_USE_SHARED_LIBRARY")
endif()

# Compile definitions
target_compile_definitions(Sierra PRIVATE "SR_LIBRARY_IMPLEMENTATION")
if(SIERRA_DEBUG_BUILD)
    target_compile_definitions(Sierra PUBLIC "SR_DEBUG")
endif()

if(SIERRA_ENABLE_LOGGING)
    target_compile_definitions(Sierra PUBLIC "SR_ENABLE_LOGGING")
endif()

# Link precompiled headers
target_precompile_headers(Sierra PUBLIC srpch.h)