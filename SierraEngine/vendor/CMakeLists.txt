target_include_directories(SierraEngine PRIVATE stb)
target_sources(SierraEngine PRIVATE stb/stb_image.h stb/stb_image_resize2.h)

set(KTX_FEATURE_STATIC_LIBRARY ON)
set(KTX_FEATURE_TESTS OFF)
add_subdirectory(KTX-Software)
target_link_libraries(SierraEngine PRIVATE ktx)
target_include_directories(SierraEngine PRIVATE KTX-Software/include KTX-Software/lib)