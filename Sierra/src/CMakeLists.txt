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
elseif(SIERRA_PLATFORM_macOS)
    set_target_properties(Sierra PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${SIERRA_APPLICATION_NAME}.app/Contents/Frameworks/")
elseif(SIERRA_BUILD_XCODE_PROJECT)
    set_target_properties(Sierra PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
endif()
#message(FATAL_ERROR ${CMAKE_BUILD_TYPE})

# Link engine modules
add_subdirectory(Core)
add_subdirectory(Engine)
add_subdirectory(Events)
add_subdirectory(Rendering)

# Link engine entry point
target_sources(Sierra PRIVATE
    Application.h
    Application.cpp
    ../include/Sierra.h
)

# Export engine module symbols
if(SIERRA_BUILD_STATIC_LIBRARY)
    target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_BUILD_STATIC_LIBRARY")
elseif(SIERRA_BUILD_SHARED_LIBRARY)
    target_compile_definitions(Sierra PRIVATE "SR_BUILD_SHARED_LIBRARY")
    target_compile_definitions(Sierra PRIVATE "SR_USE_SHARED_LIBRARY")
    target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_USE_SHARED_LIBRARY")
endif()

# Compile definitions
target_compile_definitions(Sierra PRIVATE "SR_LIBRARY_IMPLEMENTATION")
if(SIERRA_DEBUG_BUILD)
    target_compile_definitions(Sierra PRIVATE "SR_DEBUG")
    target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_DEBUG")
endif()

if(SIERRA_ENABLE_LOGGING)
    target_compile_definitions(Sierra PRIVATE "SR_ENABLE_LOGGING")
    target_compile_definitions(${SIERRA_APPLICATION_NAME} PRIVATE "SR_ENABLE_LOGGING")
endif()

# Link precompiled headers
target_precompile_headers(Sierra PRIVATE srpch.h)