target_sources(SierraEngineEditor PRIVATE
    EditorAssetManager.cpp
    EditorAssetManager.h
    AssetSerializer.h
)

add_subdirectory(Serializers)
add_subdirectory(Textures)