# Link modules
add_subdirectory(Assets)
add_subdirectory(Core)
add_subdirectory(Rendering)
add_subdirectory(Editor)

# Link precompiled headers
target_precompile_headers(SierraEngineEditor PRIVATE pch.h)

set(EASY_ARGS_DIRECTORY_PATH "../vendor/easy-args")
target_include_directories(SierraEngineEditor PRIVATE ${EASY_ARGS_DIRECTORY_PATH}/includes/)