set(MESHOPTIMIZER_DIRECTORY_PATH ../../../../../SierraEngine/vendor/meshoptimizer)
set(MESHOPT_INSTALL OFF)
# NOTE: Library lives outside of the editor's sources, so where it is built is spelled out, as a relative path would resolve outside of the build directory
add_subdirectory(${MESHOPTIMIZER_DIRECTORY_PATH} ${CMAKE_BINARY_DIR}/vendor/meshoptimizer)
target_link_libraries(SierraEngineEditor PRIVATE meshoptimizer)

target_sources(SierraEngineEditor PRIVATE
    ZeuxMeshOptimizer.cpp
    ZeuxMeshOptimizer.h
)