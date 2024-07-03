set(STB_DIRECTORY_PATH ../../vendor/stb)
target_include_directories(SierraEngine PRIVATE ${STB_DIRECTORY_PATH})
target_sources(SierraEngine PRIVATE ${STB_DIRECTORY_PATH}/stb_image.h ${STB_DIRECTORY_PATH}/stb_image_resize2.h)

target_sources(SierraEngine PRIVATE
    AssetID.cpp
    AssetID.h
    AssetManager.cpp
    AssetManager.h
)