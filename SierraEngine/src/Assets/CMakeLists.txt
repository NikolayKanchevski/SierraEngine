target_sources(SierraEngine PRIVATE
    Asset.h
    AssetManager.cpp
    AssetManager.h
    ImageSupercompressor.cpp
    ImageSupercompressor.h
    ImageTranscoder.cpp
    ImageTranscoder.h
    TextureAsset.h
    TextureImporter.cpp
    TextureImporter.h
)

add_subdirectory(Transcoders)
if(SIERRA_ENGINE_BUILD_STANDALONE)
    add_subdirectory(Platform/Standalone)
else()
    add_subdirectory(Platform/Editor)
endif()