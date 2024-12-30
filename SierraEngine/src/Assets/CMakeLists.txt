target_sources(SierraEngine PRIVATE
    Asset.h
    AssetHeader.h
    AssetID.h
    AssetImporter.cpp
    AssetImporter.h
    AssetManager.cpp
    AssetManager.h
    AssetMetadata.h
    AssetSerializer.cpp
    AssetSerializer.h
)

add_subdirectory(Materials)
add_subdirectory(Textures)