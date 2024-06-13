# Link modules
add_subdirectory(Core)

# Link precompiled headers
target_precompile_headers(SierraEngineRuntime PRIVATE pch.h)