//
// Created by Nikolay Kanchevski on 20.02.24.
//

#include "AssetManager.h"

#include "Importers/TextureImporter.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    AssetManager::AssetManager(const AssetManagerCreateInfo &createInfo)
        : threadPool(createInfo.threadPool), fileManager(createInfo.fileManager), renderingContext(createInfo.renderingContext)
    {
        ImportTexture(fileManager.GetResourcesDirectoryPath() / "core/assets/textures/DefaultCheckered.jpg.texture", [this](const AssetID assetID) -> void
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default checkered texture!");
            defaultCheckeredTexture = assetID;
        });

        ImportTexture(fileManager.GetResourcesDirectoryPath() / "core/assets/textures/DefaultBlack.jpg.texture", [this](const AssetID assetID) -> void
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default black texture!");
            defaultBlackTexture = assetID;
        });

        ImportTexture(fileManager.GetResourcesDirectoryPath() / "core/assets/textures/DefaultNormal.jpg.texture", [this](const AssetID assetID) -> void
        {
            APP_ERROR_IF(!assetID.GetHash(), "Could not import default normal texture!");
            defaultNormalTexture = assetID;
        });

        // We must wait for default assets to get loaded before proceeding, so if future asset loading fails, defaults can be returned
        threadPool.WaitForTasks();
    }

    /* --- GETTER METHODS --- */

    AssetID AssetManager::GetDefaultTexture(const TextureType textureType) const
    {
        switch (textureType)
        {
            case TextureType::Undefined:
            case TextureType::Diffuse:
            case TextureType::Skybox:           return defaultCheckeredTexture;
            case TextureType::Specular:
            case TextureType::Height:           return defaultBlackTexture;
            case TextureType::Normal:           return defaultNormalTexture;
        }

        return defaultCheckeredTexture;
    }

    std::optional<std::reference_wrapper<const Texture>> AssetManager::GetTexture(const AssetID ID) const
    {
        return textureBackend.pool.GetResource(ID);
    }

    /* --- POLLING METHODS --- */

    void AssetManager::Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer)
    {
        // Load imported textures
        while (true)
        {
            std::unique_lock textureQueueLock(textureBackend.queueMutex);
            if (textureBackend.queue.empty()) break;

            auto textureEntry = std::move(textureBackend.queue.front());
            textureBackend.queue.pop();

            textureQueueLock.unlock();

            textureBackend.pool.AddResource(textureEntry.ID, Texture({ .commandBuffer = commandBuffer, .importedTexture = std::move(textureEntry.importedAsset) }));
            textureEntry.LoadCallback(textureEntry.ID);
        }
    }

    void AssetManager::ImportTexture(const std::filesystem::path &assetFilePath, const AssetLoadCallback &LoadCallback)
    {
        const AssetID ID = assetFilePath;
        if (textureBackend.pool.ResourceExists(ID))
        {
            LoadCallback(ID);
            return;
        }

        threadPool.PushTask([this, LoadCallback, ID, assetFilePath]() -> void
        {
            if (std::optional<Sierra::File> assetFile = fileManager.OpenFile(assetFilePath, Sierra::FileAccess::ReadOnly); assetFile.has_value())
            {
                // Load asset file
                const std::vector<uint8> assetFileMemory = assetFile.value().Read();
                const SerializedTexture &serializedTexture = *reinterpret_cast<const SerializedTexture*>(assetFileMemory.data());

                // Check if invalid or outdated
                const TextureImporter textureImporter = TextureImporter({ });
                if (!serializedTexture.header.IsValid())
                {
                    APP_WARNING("Cannot import texture [{0}], as it is invalid.", assetFilePath.string());
                    LoadCallback(GetDefaultTexture(serializedTexture.index.type));
                    return;
                }
                if (serializedTexture.header.IsOutdated(textureImporter.GetVersion()))
                {
                    APP_WARNING("Cannot import texture [{0}], as its asset version [{1}.{2}.{3}], is incompatible with that of importer - [{4}.{5}.{6}].", assetFilePath.string(), serializedTexture.header.version.GetMajor(), serializedTexture.header.version.GetMinor(), serializedTexture.header.version.GetPatch(), textureImporter.GetVersion().GetMajor(), textureImporter.GetVersion().GetMinor(), textureImporter.GetVersion().GetPatch());
                    LoadCallback(GetDefaultTexture(serializedTexture.index.type));
                    return;
                }

                // Import texture and on fail return default
                std::optional<ImportedTexture> importedTexture = textureImporter.Import({ .name = assetFilePath.filename().string(), .renderingContext = renderingContext }, assetFileMemory);
                if (!importedTexture.has_value())
                {
                    APP_WARNING("Could not import texture [{0}]!", assetFilePath.string());
                    LoadCallback(GetDefaultTexture(serializedTexture.index.type));
                    return;
                }

                // Add texture data to queue
                const std::lock_guard textureQueueLock(textureBackend.queueMutex);
                textureBackend.queue.push({ .ID = ID, .importedAsset = std::move(importedTexture.value()), .LoadCallback = LoadCallback });
            }
            else
            {
                APP_ERROR("Cannot import texture [{0}], as it could not be open for reading! Make sure its file path is valid and the asset file is not in a protected directory.", assetFilePath.string());
            }
        });
    }

}