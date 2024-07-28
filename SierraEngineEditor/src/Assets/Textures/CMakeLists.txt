target_sources(SierraEngineEditor PRIVATE
    TextureSerializer.h
    Image.h
    Image.cpp
    ImageCompressor.h
    ImageTranscoder.h
)

add_subdirectory(Compressors)
add_subdirectory(Images)
add_subdirectory(Serializers)
add_subdirectory(Transcoders)