# Link modules
add_subdirectory(Assets)
add_subdirectory(Core)
add_subdirectory(Editor)
add_subdirectory(Platform)
add_subdirectory(Rendering)

# Link precompiled headers
target_precompile_headers(SierraEngineEditor PRIVATE pch.h)

set(EASY_ARGS_DIRECTORY_PATH "../vendor/easy-args")
target_include_directories(SierraEngineEditor PRIVATE ${EASY_ARGS_DIRECTORY_PATH}/includes/)