# Generate library
if((SIERRA_BUILD_STATIC_LIBRARY AND SIERRA_BUILD_SHARED_LIBRARY) OR (NOT SIERRA_BUILD_STATIC_LIBRARY AND NOT SIERRA_BUILD_SHARED_LIBRARY))
    message(FATAL_ERROR "[Sierra]: Illegal build configuration! You must compile with either SIERRA_BUILD_STATIC_LIBRARY or SIERRA_BUILD_SHARED_LIBRARY set, but not both!")
endif()

if(SIERRA_BUILD_STATIC_LIBRARY)
    message(STATUS "[Sierra]: Building Sierra as static library...")
    add_library(Sierra STATIC)
elseif(SIERRA_BUILD_SHARED_LIBRARY)
    message(STATUS "[Sierra]: Building Sierra as dynamic library...")
    add_library(Sierra SHARED)
endif()

# Set up library output path
set(SIERRA_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/")
if(SIERRA_PLATFORM_macOS)
    set(SIERRA_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/${SIERRA_APPLICATION_NAME}.app/Contents/Frameworks/")
elseif(SIERRA_PLATFORM_iOS)
    set(SIERRA_LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin/")
endif()

# Assign library output path
set_target_properties(Sierra PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${SIERRA_LIBRARY_OUTPUT_PATH}
    LIBRARY_OUTPUT_DIRECTORY ${SIERRA_LIBRARY_OUTPUT_PATH}
    RUNTIME_OUTPUT_DIRECTORY ${SIERRA_LIBRARY_OUTPUT_PATH}
)

# Link engine modules
add_subdirectory(Core)
add_subdirectory(Utilities)
add_subdirectory(Rendering)
add_subdirectory(Extensions)

# Link include files
target_sources(Sierra PRIVATE
    srpch.h
    ../include/Sierra.h
)

# Export engine module symbols
if(SIERRA_BUILD_SHARED_LIBRARY)
    target_compile_definitions(Sierra PRIVATE "SR_BUILD_SHARED_LIBRARY")
    target_compile_definitions(Sierra PUBLIC "SR_USE_SHARED_LIBRARY")
endif()

# Compile definitions
target_compile_definitions(Sierra PRIVATE "SR_LIBRARY_IMPLEMENTATION")
if(SIERRA_ENABLE_LOGGING)
    target_compile_definitions(Sierra PUBLIC "SR_ENABLE_LOGGING")
endif()