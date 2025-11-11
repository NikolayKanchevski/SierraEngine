//
// Created by Nikolay Kanchevski on 9.10.25.
//

#include "EditorAssetManager.h"

#include "Textures/Importers/AutoTextureImporter.h"

namespace SierraEngine
{

    constexpr std::string_view ASSET_BLOB_EXTENSION = ".blob";

    const std::unordered_map<std::string_view, AssetType> ASSET_FILE_EXTENSION_MAP
    {
        { ".texture", AssetType::Texture },
        { ".material", AssetType::Material }
    };

    /* --- CONSTRUCTORS --- */

    EditorAssetManager::EditorAssetManager(const AssetManagerCreateInfo &createInfo)
        : AssetManager(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    void EditorAssetManager::Update(Sierra::CommandBuffer& commandBuffer)
    {

    }

    void EditorAssetManager::LoadProjectAssets(const Sierra::FileManager& fileManager, const Project& project)
    {
        const std::filesystem::path assetDirectoryPath = project.GetAssetPath();
        fileManager.EnumerateDirectoryFiles(assetDirectoryPath, true, [this, &fileManager](const std::filesystem::path& filePath) -> void
        {
            if (!filePath.has_extension())
                return;

            const std::string extension = filePath.extension().string();
            if (const auto iterator = ASSET_FILE_EXTENSION_MAP.find(extension); iterator != ASSET_FILE_EXTENSION_MAP.end())
            {
                std::vector<uint8> assetData = fileManager.ReadFile(filePath.string());
                switch (iterator->second)
                {
                    case AssetType::Texture:
                    {
                        const std::filesystem::path blobFilePath = std::filesystem::path(filePath).replace_extension(ASSET_BLOB_EXTENSION);
                        if (!fileManager.FileExists(blobFilePath))
                        {
                            APP_WARNING("Asset manager cannot load asset [{0}], as its corresponding memory [.blob] file [{1}] is missing", filePath.string(), blobFilePath.string());
                            return;
                        }

                        std::vector<uint8> assetBlob = fileManager.ReadFile(blobFilePath);
                        const SerializedTexture serializedTexture
                        {
                            .data = std::move(assetData),
                            .blob = std::move(assetBlob)
                        };

                        const std::optional<ImportedTexture> importedTexture = AutoTextureImporter::Import({
                            .serializedTexture = serializedTexture,
                            .format = Sierra::ImageFormat::R8G8B8A8_UNorm
                        });

                        if (!importedTexture.has_value())
                        {
                            APP_WARNING("Asset manager could not load asset [{0}]", filePath.string());
                            return;
                        }

                        ImportTexture(importedTexture.value());
                        APP_INFO("Texture Imported - [{0}]", filePath.string());
                        break;
                    }
                    case AssetType::Shader:
                    case AssetType::Material:
                    case AssetType::Model:
                    case AssetType::Unknown:
                    {
                        return;
                    }
                }
            }
        });
    }

    void EditorAssetManager::ImportTexture(const ImportedTexture& importedTexture)
    {

    }

    void EditorAssetManager::ImportMaterial(const ImportedMaterial& importedMaterial)
    {

    }

}
