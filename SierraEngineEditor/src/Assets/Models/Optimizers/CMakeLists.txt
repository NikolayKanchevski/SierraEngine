set(MESHOPTIMIZER_DIRECTORY_PATH ../../../../../SierraEngine/vendor/meshoptimizer)
set(MESHOPT_INSTALL OFF)
add_subdirectory(${MESHOPTIMIZER_DIRECTORY_PATH} ${MESHOPTIMIZER_DIRECTORY_PATH})
target_link_libraries(SierraEngineEditor PRIVATE meshoptimizer)

target_sources(SierraEngineEditor PRIVATE
    ZeuxMeshOptimizer.cpp
    ZeuxMeshOptimizer.h
)