set(KTX_DIRECTORY_PATH ../../../../../vendor/KTX-Software)
set(KTX_FEATURE_STATIC_LIBRARY ON)
set(KTX_FEATURE_TESTS OFF)
add_subdirectory(${KTX_DIRECTORY_PATH} ${KTX_DIRECTORY_PATH})
target_link_libraries(SierraEngine PRIVATE ktx)
target_include_directories(SierraEngine PRIVATE ${KTX_DIRECTORY_PATH}/include/)

target_sources(SierraEngine PRIVATE
        KTXTranscoder.cpp
        KTXTranscoder.h
)