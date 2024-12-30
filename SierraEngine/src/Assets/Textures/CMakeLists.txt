target_sources(SierraEngine PRIVATE
    Image.cpp
    Image.h
    ImageCompressor.h
    ImageConverter.cpp
    ImageConverter.h
    ImageLoader.h
    ImageTranscoder.h
    TextureAsset.cpp
    TextureAsset.h
    TextureImporter.cpp
    TextureImporter.h
    TextureSerializer.cpp
    TextureSerializer.h
)

add_subdirectory(Compressors)
add_subdirectory(Transcoders)