# Link modules
add_subdirectory(Core)
add_subdirectory(Extensions)
add_subdirectory(Rendering)

# Link include files
target_sources(Sierra PRIVATE
    srpch.h
    ../include/Sierra/Sierra.h
)

# Link precompiled headers
target_precompile_headers(Sierra PRIVATE "${SIERRA_DIRECTORY_PATH}/src/srpch.h")

# Export engine module symbols
if(SIERRA_BUILD_SHARED_LIBRARY)
    target_compile_definitions(Sierra PRIVATE "SR_BUILD_SHARED_LIBRARY")
    target_compile_definitions(Sierra PUBLIC "SR_USE_SHARED_LIBRARY")
endif()

# Compile definitions
target_compile_definitions(Sierra PRIVATE "SR_LIBRARY_IMPLEMENTATION")
if(SIERRA_ENABLE_LOGGING)
    target_compile_definitions(Sierra PUBLIC "SR_ENABLE_LOGGING=1")
else()
    target_compile_definitions(Sierra PUBLIC "SR_ENABLE_LOGGING=0")
endif()