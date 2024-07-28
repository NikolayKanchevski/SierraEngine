# Link modules
add_subdirectory(Assets)
add_subdirectory(Core)
add_subdirectory(Rendering)
add_subdirectory(Editor)

# Link precompiled headers
target_precompile_headers(SierraEngineEditor PRIVATE pch.h)