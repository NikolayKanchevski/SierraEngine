target_sources(SierraEngine PRIVATE
    ImageAccessor.h
    ImageAccessor.cpp
    ImageCompressor.h
    ImageConverter.cpp
    ImageConverter.h
    ImageLoader.h
    ImageTranscoder.h
    TextureImporter.cpp
    TextureImporter.h
    TextureSerializer.cpp
    TextureSerializer.h
)

add_subdirectory(Compressors)
add_subdirectory(Transcoders)