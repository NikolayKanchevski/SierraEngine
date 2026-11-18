set(BASIS_UNIVERSAL_DIRECTORY_PATH ../../../../vendor/basis_universal)
set(BASISU_STATIC ON)
set(BASISU_ZSTD OFF)
set(BASISU_OPENCL OFF)
set(BASISU_EXAMPLES OFF)
# NOTE: Only the encoder library is built, as the library places its executables (such as its command-line tool) within its own sources
add_subdirectory(${BASIS_UNIVERSAL_DIRECTORY_PATH} ${BASIS_UNIVERSAL_DIRECTORY_PATH} EXCLUDE_FROM_ALL)
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