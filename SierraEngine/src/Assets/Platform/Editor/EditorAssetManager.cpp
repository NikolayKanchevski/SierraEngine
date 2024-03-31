//
// Created by Nikolay Kanchevski on 20.02.24.
//

#include "EditorAssetManager.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    EditorAssetManager::EditorAssetManager(ThreadPool &threadPool, const Sierra::RenderingContext &renderingContext, const AssetManagerCreateInfo &createInfo)
        : AssetManager(createInfo), threadPool(threadPool), renderingContext(renderingContext),
          textureSerializer({ .version = createInfo.version, .maximumTextureDimensions = MAX_TEXTURE_DIMENSIONS }), textureImporter({ .version = createInfo.version }), texturePool({ .initialSize = 256 })
    {
        ImportTexture(Sierra::File::GetResourcesDirectoryPath() / "core/assets/textures/DefaultBlack.jpg", [this](const AssetID assetID)
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default black texture!");
            defaultTextures[static_cast<uint8>(TextureType::Undefined)] = assetID;
            defaultTextures[static_cast<uint8>(TextureType::Specular)] = assetID;
            defaultTextures[static_cast<uint8>(TextureType::Height)] = assetID;
        });

        ImportTexture(Sierra::File::GetResourcesDirectoryPath() / "core/assets/textures/DefaultDiffuse.jpg", [this](const AssetID assetID)
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default diffuse texture!");
            defaultTextures[static_cast<uint8>(TextureType::Diffuse)] = assetID;
        });

        ImportTexture(Sierra::File::GetResourcesDirectoryPath() / "core/assets/textures/DefaultNormal.jpg", [this](const AssetID assetID)
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default normal texture!");
            defaultTextures[static_cast<uint8>(TextureType::Normal)] = assetID;
        });

        // We must wait for default assets to get loaded before proceeding, so if future asset loading fails, defaults can be returned
        threadPool.WaitForTasks();
    }

    /* --- POLLING METHODS --- */

    void EditorAssetManager::Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer)
    {
        const std::lock_guard textureQueueLock(textureQueueMutex);
        while (!textureQueue.empty())
        {
            AssetQueueEntry<TextureAsset> textureEntry = std::move(textureQueue.front());
            textureQueue.pop();

            commandBuffer->SynchronizeImageUsage(textureEntry.asset.GetImage(), Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::MemoryWrite);
            commandBuffer->CopyBufferToImage(textureEntry.buffer, textureEntry.asset.GetImage());

            commandBuffer->SynchronizeImageUsage(textureEntry.asset.GetImage(), Sierra::ImageCommandUsage::MemoryWrite, Sierra::ImageCommandUsage::GraphicsRead);
            commandBuffer->QueueBufferForDestruction(std::move(textureEntry.buffer));

            texturePool.AddResource(textureEntry.ID, std::move(textureEntry.asset));
            textureEntry.LoadCallback(textureEntry.ID);
        }
    }

    bool EditorAssetManager::SerializeTexture(const std::filesystem::path &filePath, const TextureSerializeInfo &serializeInfo)
    {
        // Serialize texture
        auto serializedTexture = textureSerializer.Serialize(filePath, serializeInfo);
        if (!serializedTexture.has_value())
        {
            APP_WARNING("Could not serialize texture [{0}]!", filePath.string());
            return false;
        }

        const std::filesystem::path assetFilePath = filePath.string() + TEXTURE_ASSET_EXTENSION;
        const auto [textureMetadata, content] = std::move(serializedTexture.value());

        // Write serialized memory and raw texture data to asset file
        Sierra::File::WriteToFile(assetFilePath, &textureMetadata, sizeof(SerializedTexture), 0, 0);
        Sierra::File::WriteToFile(assetFilePath, content, textureMetadata.contentMemorySize, 0, sizeof(SerializedTexture));
        std::free(content);

        return true;
    }

    void EditorAssetManager::ImportTexture(const std::filesystem::path &filePath, const AssetLoadCallback LoadCallback)
    {
        const std::filesystem::path assetFilePath = filePath.string() + TEXTURE_ASSET_EXTENSION;

        const AssetID ID = assetFilePath;
        if (texturePool.ResourceExists(ID))
        {
            LoadCallback(ID);
            return;
        }

        threadPool.PushTask([this, LoadCallback, ID, filePath, assetFilePath]() -> void
        {
            SerializedTexture serializedTexture = { };

            Import:
                while (Sierra::File::FileExists(assetFilePath))
                {
                    // Load asset file
                    const std::vector<uint8> assetFileMemory = Sierra::File::ReadFile(assetFilePath);
                    serializedTexture = *reinterpret_cast<const SerializedTexture*>(assetFileMemory.data());
                    const void* textureContent = assetFileMemory.data() + sizeof(SerializedTexture);

                    // Check if outdated
                    if (serializedTexture.version < textureImporter.GetVersion())
                    {
                        APP_INFO("Re-importing texture [{0}], as its asset format [{1}.{2}.{3}], is incompatible with that of importer - [{4}.{5}.{6}].", assetFilePath.string(), serializedTexture.version.GetMajor(), serializedTexture.version.GetMinor(), serializedTexture.version.GetPatch(), textureImporter.GetVersion().GetMajor(), textureImporter.GetVersion().GetMinor(), textureImporter.GetVersion().GetPatch());
                        Sierra::File::DeleteFile(assetFilePath);
                        break;
                    }

                    // Import texture and on fail return default
                    auto importedTexture = textureImporter.Import(renderingContext, filePath.filename().string(), std::make_pair(serializedTexture, textureContent));
                    if (!importedTexture.has_value())
                    {
                        APP_WARNING("Could not import texture [{0}]!", assetFilePath.string());
                        LoadCallback(GetDefaultTexture(serializedTexture.type));
                        return;
                    }

                    auto [textureAsset, buffer] = std::move(importedTexture.value());

                    // Add texture data to queue
                    const std::lock_guard textureQueueLock(textureQueueMutex);
                    textureQueue.push({ ID, std::move(textureAsset), std::move(buffer), LoadCallback });

                    return;
                }

            // Re-use outdated info and define serialization info
            const TextureSerializeInfo serializeInfo
            {
                .type = serializedTexture.type,
                .filtering = serializedTexture.filtering,
                .compressorType = serializedTexture.compressorType != ImageSupercompressorType::Undefined ? serializedTexture.compressorType : ImageSupercompressorType::KTX,
                .compressionLevel = ImageSupercompressionLevel::Standard,
                .qualityLevel = ImageSupercompressionQualityLevel::Standard
            };

            // Re-serialize and re-import texture
            if (!SerializeTexture(filePath, serializeInfo))
            {
                LoadCallback(GetDefaultTexture(serializedTexture.type));
                return;
            }

            goto Import;
        });
    }

    EditorAssetManager::~EditorAssetManager()
    {
        threadPool.WaitForTasks();
    }

}