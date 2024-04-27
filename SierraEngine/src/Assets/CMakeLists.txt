set(STB_DIRECTORY_PATH ../../vendor/stb)
target_include_directories(SierraEngine PRIVATE ${STB_DIRECTORY_PATH})
target_sources(SierraEngine PRIVATE ${STB_DIRECTORY_PATH}/stb_image.h ${STB_DIRECTORY_PATH}/stb_image_resize2.h)

target_sources(SierraEngine PRIVATE
    AssetID.hpp
    AssetManager.cpp
    AssetManager.h
    SerializedAsset.h
    SerializedScene.h
    SerializedTexture.h
)

add_subdirectory(Importers)
if(NOT SIERRA_ENGINE_BUILD_STANDALONE)
    add_subdirectory(Platform/Editor)
endif()