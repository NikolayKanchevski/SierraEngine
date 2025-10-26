target_sources(SierraEngineEditor PRIVATE
    EditorAssetManager.cpp
    EditorAssetManager.h
)

add_subdirectory(Importers)
add_subdirectory(Materials)
add_subdirectory(Serializers)
add_subdirectory(Textures)