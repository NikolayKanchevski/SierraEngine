target_sources(SierraEngineEditor PRIVATE
    ImageCompressor.h
    ImageTranscoder.h
    RawImage.cpp
    RawImage.h
    TextureSerializer.h
)

add_subdirectory(Compressors)
add_subdirectory(Images)
add_subdirectory(Serializers)
add_subdirectory(Transcoders)