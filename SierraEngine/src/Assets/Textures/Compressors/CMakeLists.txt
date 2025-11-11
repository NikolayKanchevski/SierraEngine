set(BASIS_UNIVERSAL_DIRECTORY_PATH ../../../../vendor/basis_universal)
set(STATIC ON)
set(ZSTD OFF)
set(OPENCL OFF)
add_subdirectory(${BASIS_UNIVERSAL_DIRECTORY_PATH} ${BASIS_UNIVERSAL_DIRECTORY_PATH})
target_link_libraries(SierraEngine PRIVATE basisu_encoder)
target_include_directories(SierraEngine SYSTEM PUBLIC ${BASIS_UNIVERSAL_DIRECTORY_PATH})

target_sources(SierraEngine PRIVATE
    BasisUniversalCompressor.cpp
    BasisUniversalCompressor.h
)
set_source_files_properties(
    BasisUniversalCompressor.cpp
    BasisUniversalCompressor.h
    PROPERTIES COMPILE_FLAGS "-w"
)