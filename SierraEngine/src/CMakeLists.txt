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