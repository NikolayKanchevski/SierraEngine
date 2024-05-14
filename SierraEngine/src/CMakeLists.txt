if((SIERRA_ENGINE_BUILD_STATIC_LIBRARY AND SIERRA_ENGINE_BUILD_SHARED_LIBRARY) OR (NOT SIERRA_ENGINE_BUILD_STATIC_LIBRARY AND NOT SIERRA_ENGINE_BUILD_SHARED_LIBRARY))
    message(FATAL_ERROR "[Sierra]: Illegal build configuration! You must compile with either SIERRA_ENGINE_BUILD_STATIC_LIBRARY or SIERRA_ENGINE_BUILD_SHARED_LIBRARY set, but not both!")
endif()

# Generate library
if(SIERRA_ENGINE_BUILD_STATIC_LIBRARY)
    message(STATUS "[Sierra Engine]: Building Sierra as static library...")
    add_library(SierraEngine STATIC)
elseif(SIERRA_ENGINE_BUILD_SHARED_LIBRARY)
    message(STATUS "[Sierra Engine]: Building Sierra Engine as dynamic library...")
    add_library(SierraEngine SHARED)
endif()

# Set up library output path
set(SIERRA_ENGINE_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/")
if(SIERRA_ENGINE_PLATFORM_macOS)
    set(SIERRA_ENGINE_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${SIERRA_ENGINE_APPLICATION_NAME}.app/Contents/Frameworks/")
elseif(SIERRA_ENGINE_PLATFORM_iOS)
    set(SIERRA_ENGINE_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin/")
endif()

# Assign library output path
set_target_properties(SierraEngine PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${SIERRA_ENGINE_LIBRARY_OUTPUT_PATH}
    LIBRARY_OUTPUT_DIRECTORY ${SIERRA_ENGINE_LIBRARY_OUTPUT_PATH}
    RUNTIME_OUTPUT_DIRECTORY ${SIERRA_ENGINE_LIBRARY_OUTPUT_PATH}
)

# Link modules
add_subdirectory(Core)
add_subdirectory(Assets)
add_subdirectory(Rendering)
add_subdirectory(Scene)

# Link include files
target_sources(SierraEngine PRIVATE
    srepch.h
    ../include/SierraEngine/SierraEngine.h
)

# Link precompiled headers
target_precompile_headers(SierraEngine PRIVATE "${SIERRA_ENGINE_DIRECTORY_PATH}/src/srepch.h")

# Export engine module symbols
if(SIERRA_ENGINE_BUILD_SHARED_LIBRARY)
    target_compile_definitions(SierraEngine PRIVATE "SRE_BUILD_SHARED_LIBRARY")
    target_compile_definitions(SierraEngine PUBLIC "SRE_USE_SHARED_LIBRARY")
endif()

# Compile definitions
target_compile_definitions(SierraEngine PRIVATE "SRE_LIBRARY_IMPLEMENTATION")