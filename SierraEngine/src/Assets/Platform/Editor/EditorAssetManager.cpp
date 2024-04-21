//
// Created by Nikolay Kanchevski on 20.02.24.
//

#include "EditorAssetManager.h"

#include "TextureSerializer.h"
#include "../../TextureImporter.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    EditorAssetManager::EditorAssetManager(const AssetManagerCreateInfo &createInfo)
        : AssetManager(createInfo), texturePool({ .initialSize = 256 })
    {
        ImportTexture(GetFileManager().GetResourcesDirectoryPath() / "core/assets/textures/DefaultCheckered.jpg.texture", [this](const AssetID assetID) -> void
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default checkered texture!");
            defaultCheckeredTexture = assetID;
        });

        ImportTexture(GetFileManager().GetResourcesDirectoryPath() / "core/assets/textures/DefaultBlack.jpg.texture", [this](const AssetID assetID) -> void
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default black texture!");
            defaultBlackTexture = assetID;
        });

        ImportTexture(GetFileManager().GetResourcesDirectoryPath() / "core/assets/textures/DefaultNormal.jpg.texture", [this](const AssetID assetID) -> void
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default normal texture!");
            defaultNormalTexture = assetID;
        });

        // We must wait for default assets to get loaded before proceeding, so if future asset loading fails, defaults can be returned
        GetThreadPool().WaitForTasks();
    }

    /* --- GETTER METHODS --- */

    AssetID EditorAssetManager::GetDefaultTexture(const TextureType textureType) const
    {
        switch (textureType)
        {
            case TextureType::Undefined:
            case TextureType::Diffuse:
            default:                            break;
            case TextureType::Specular:
            case TextureType::Height:           return defaultBlackTexture;
            case TextureType::Normal:           return defaultNormalTexture;
        }

        return defaultCheckeredTexture;
    }

    /* --- POLLING METHODS --- */

    void EditorAssetManager::Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer)
    {
        // Save loaded textures
        {
            const std::lock_guard textureQueueLock(textureQueueMutex);
            while (!textureQueue.empty())
            {
                TextureAssetQueueEntry textureEntry = std::move(textureQueue.front());
                textureQueue.pop();

                // Write every level to image
                commandBuffer->SynchronizeImageUsage(textureEntry.asset.GetImage(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::MemoryWrite);
                for (uint32 level = 0; level < textureEntry.asset.GetImage()->GetLevelCount(); level++)
                {
                    for (uint32 layer = 0; layer < textureEntry.asset.GetImage()->GetLayerCount(); layer++)
                    {
                        commandBuffer->CopyBufferToImage(textureEntry.data[level], textureEntry.asset.GetImage(), level, layer, { 0, 0 }, layer * (textureEntry.data[level]->GetMemorySize() / textureEntry.asset.GetImage()->GetLayerCount()));
                    }
                    commandBuffer->QueueBufferForDestruction(std::move(textureEntry.data[level]));
                }
                commandBuffer->SynchronizeImageUsage(textureEntry.asset.GetImage(), Sierra::ImageCommandUsage::MemoryWrite, Sierra::ImageCommandUsage::GraphicsRead);

                texturePool.AddResource(textureEntry.ID, std::move(textureEntry.asset));
                textureEntry.LoadCallback(textureEntry.ID);
            }
        }
    }

    bool EditorAssetManager::SerializeTexture(const std::initializer_list<std::initializer_list<std::filesystem::path>> &levelFilePaths, const TextureSerializeInfo &serializeInfo)
    {
        if (levelFilePaths.begin()->size() == 0 || levelFilePaths.begin()->begin()->empty())
        {
            APP_WARNING("Cannot serialize texture with no file paths provided!");
            return false;
        }

        // Serialize texture
        const std::optional<std::pair<SerializedTexture, SerializedTextureBlob>> serializedTexture = TextureSerializer({ .maxTextureDimensions = MAX_TEXTURE_DIMENSIONS }).Serialize(GetFileManager(), levelFilePaths, serializeInfo);
        if (!serializedTexture.has_value())
        {
            APP_WARNING("Could not serialize texture [{0}]!", levelFilePaths.begin()->begin()->string());
            return false;
        }

        const std::filesystem::path assetFilePath = levelFilePaths.begin()->begin()->string() + std::string(TEXTURE_ASSET_EXTENSION);
        std::optional<Sierra::File> assetFile = GetFileManager().CreateAndOpenFile(assetFilePath, Sierra::FileAccess::WriteOnly);
        APP_ERROR_IF(!assetFile.has_value(), "Could not write serialized texture to asset file [{0}]!", assetFilePath.string());

        // Write serialized memory and raw texture data to asset file
        const auto &[metadata, blob] = serializedTexture.value();
        assetFile.value().Resize(sizeof(SerializedTexture) + blob.size());
        assetFile.value().Write(&metadata, sizeof(SerializedTexture));
        assetFile.value().Write(blob.data(), blob.size());

        return true;
    }

    void EditorAssetManager::ImportTexture(const std::filesystem::path &assetFilePath, const AssetLoadCallback LoadCallback)
    {
        const AssetID ID = assetFilePath;
        if (texturePool.ResourceExists(ID))
        {
            LoadCallback(ID);
            return;
        }

        GetThreadPool().PushTask([this, LoadCallback, ID, assetFilePath]() -> void
        {
            if (std::optional<Sierra::File> assetFile = GetFileManager().OpenFile(assetFilePath, Sierra::FileAccess::ReadOnly); assetFile.has_value())
            {
                // Load asset file
                const std::vector<uint8> assetFileMemory = assetFile.value().Read();
                const SerializedTexture &serializedTexture = *reinterpret_cast<const SerializedTexture*>(assetFileMemory.data());
                const uint8* textureBlob = assetFileMemory.data() + sizeof(SerializedTexture);

                // Check if outdated
                const TextureImporter textureImporter = TextureImporter({ });
                if (serializedTexture.header.version < textureImporter.GetVersion())
                {
                    APP_WARNING("Cannot import texture [{0}], as its asset version [{1}.{2}.{3}], is incompatible with that of importer - [{4}.{5}.{6}].", assetFilePath.string(), serializedTexture.header.version.GetMajor(), serializedTexture.header.version.GetMinor(), serializedTexture.header.version.GetPatch(), textureImporter.GetVersion().GetMajor(), textureImporter.GetVersion().GetMinor(), textureImporter.GetVersion().GetPatch());
                    LoadCallback(GetDefaultTexture(serializedTexture.index.type));
                    return;
                }

                // Import texture and on fail return default
                std::optional<ImportedTexture> importedTexture = textureImporter.Import(GetRenderingContext(), assetFilePath.filename().string(), serializedTexture, { textureBlob, assetFileMemory.size() - sizeof(SerializedTexture) });
                if (!importedTexture.has_value())
                {
                    APP_WARNING("Could not import texture [{0}]!", assetFilePath.string());
                    LoadCallback(GetDefaultTexture(serializedTexture.index.type));
                    return;
                }

                auto &[textureAsset, buffers] = importedTexture.value();

                // Add texture data to queue
                const std::lock_guard textureQueueLock(textureQueueMutex);
                textureQueue.push({ ID, std::move(textureAsset), std::move(buffers), LoadCallback });
            }
            else
            {
                APP_ERROR("Cannot import texture [{0}], as it could not be open for reading! Make sure its file path is valid and the asset file is not in a protected directory.", assetFilePath.string());
            }
        });
    }

}
