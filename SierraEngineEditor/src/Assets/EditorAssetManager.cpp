//
// Created by Nikolay Kanchevski on 15.07.24.
//

#include "EditorAssetManager.h"

namespace SierraEngine
{

    namespace
    {
        constexpr std::array<uint8, 96> CHECKERED_TEXTURE_MEMORY
        {
            154, 26, 27, 255,
            109, 0, 0, 255,
            113, 0, 0, 255,
            143, 15, 16, 255,

            154, 26, 27, 255,
            109, 0, 0, 255,
            113, 0, 0, 255,
            143, 15, 16, 255,

            154, 26, 27, 255,
            109, 0, 0, 255,
            113, 0, 0, 255,
            143, 15, 16, 255,

            154, 26, 27, 255,
            109, 0, 0, 255,
            113, 0, 0, 255,
            143, 15, 16, 255,

            154, 26, 27, 255,
            109, 0, 0, 255,
            113, 0, 0, 255,
            143, 15, 16, 255,

            154, 26, 27, 255,
            109, 0, 0, 255,
            113, 0, 0, 255,
            143, 15, 16, 255,
        };

        constexpr std::array<uint8, 4> BLACK_TEXTURE_MEMORY
        {
            1, 1, 1, 255
        };

        constexpr std::array<uint8, 4> NORMAL_TEXTURE_MEMORY
        {
            128, 127, 254, 255
        };
    }

    /* --- CONSTRUCTORS --- */

    EditorAssetManager::EditorAssetManager(const EditorAssetManagerCreateInfo& createInfo)
        : AssetManager(), threadPool(createInfo.threadPool), renderingContext(createInfo.renderingContext), textureMap(3)
    {
        // Default checkered texture
        {
            std::unique_ptr<Sierra::Buffer> textureBuffer = renderingContext.CreateBuffer({
                .name = "Buffer of Texture [Default Checkered]",
                .memorySize = sizeof(uint8) * CHECKERED_TEXTURE_MEMORY.size(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            textureBuffer->CopyFromMemory(CHECKERED_TEXTURE_MEMORY.data());

            defaultCheckeredTextureID = std::hash<std::string_view>{}(":default/CHECKERED_TEXTURE");
            TextureQueueEntry textureEntry
            {
                .ID = defaultCheckeredTextureID,
                .texture = Texture({
                    .name = "Default Checkered Texture",
                    .renderingContext = renderingContext,
                    .width = 2,
                    .height = 2,
                    .preferredFormat = Sierra::ImageFormat::R8G8B8A8_UNorm,
                    .layerCount = 6
                }),
                .levelBuffers = std::vector<std::unique_ptr<Sierra::Buffer>>(6)
            };

            textureEntry.levelBuffers[0] = std::move(textureBuffer);
            textureQueue.emplace(std::move(textureEntry));
        }

        // Default black texture
        {
            std::unique_ptr<Sierra::Buffer> textureBuffer = renderingContext.CreateBuffer({
                .name = "Buffer of Texture [Default Black]",
                .memorySize = sizeof(uint8) * BLACK_TEXTURE_MEMORY.size(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            textureBuffer->CopyFromMemory(BLACK_TEXTURE_MEMORY.data());

            defaultBlackTextureID = std::hash<std::string_view>{}(":default/BLACK_TEXTURE");
            TextureQueueEntry textureEntry
            {
                .ID = defaultBlackTextureID,
                .texture = Texture({
                    .name = "Default Black Texture",
                    .renderingContext = renderingContext,
                    .width = 1,
                    .height = 1,
                    .preferredFormat = Sierra::ImageFormat::R8G8B8A8_UNorm
                }),
                .levelBuffers = std::vector<std::unique_ptr<Sierra::Buffer>>(1)
            };

            textureEntry.levelBuffers[0] = std::move(textureBuffer);
            textureQueue.emplace(std::move(textureEntry));
        }

        // Default normal texture
        {
            std::unique_ptr<Sierra::Buffer> textureBuffer = renderingContext.CreateBuffer({
                .name = "Buffer of Texture [Default Normal]",
                .memorySize = sizeof(uint8) * NORMAL_TEXTURE_MEMORY.size(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            textureBuffer->CopyFromMemory(NORMAL_TEXTURE_MEMORY.data());

            defaultNormalTextureID = std::hash<std::string_view>{}(":default/NORMAL_TEXTURE");
            TextureQueueEntry textureEntry
            {
                .ID = defaultNormalTextureID,
                .texture = Texture({
                    .name = "Default Normal Texture",
                    .renderingContext = renderingContext,
                    .width = 1,
                    .height = 1,
                    .preferredFormat = Sierra::ImageFormat::R8G8B8A8_UNorm
                }),
                .levelBuffers = std::vector<std::unique_ptr<Sierra::Buffer>>(1)
            };

            textureEntry.levelBuffers[0] = std::move(textureBuffer);
            textureQueue.emplace(std::move(textureEntry));
        }
    }

    /* --- POLLING METHODS --- */

    void EditorAssetManager::Update(Sierra::CommandBuffer& commandBuffer)
    {
        while (true)
        {
            std::unique_lock textureQueueLock(textureQueueMutex);
            if (textureQueue.empty()) break;

            TextureQueueEntry textureEntry = std::move(textureQueue.front());
            textureQueue.pop();

            textureQueueLock.unlock();
            const Sierra::Image& textureImage = textureEntry.texture.GetImage();

            commandBuffer.SynchronizeImageUsage(textureImage, Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::MemoryWrite);
            for (size level = 0; level < textureEntry.texture.GetLevelCount(); level++)
            {
                std::unique_ptr<Sierra::Buffer>& levelBuffer = textureEntry.levelBuffers[level];
                for (uint32 layer = 0; layer < textureEntry.texture.GetLayerCount(); layer++)
                {
                    commandBuffer.CopyBufferToImage(*levelBuffer, textureImage, level, layer, { 0, 0, 0 }, layer * (levelBuffer->GetMemorySize() / textureImage.GetLayerCount()));
                }
                commandBuffer.QueueBufferForDestruction(std::move(levelBuffer));
            }
            commandBuffer.SynchronizeImageUsage(textureImage, Sierra::ImageCommandUsage::MemoryWrite, Sierra::ImageCommandUsage::GraphicsRead);

            textureMap.emplace(textureEntry.ID, std::move(textureEntry.texture));
            textureEntry.LoadCallback(textureEntry.ID);
        }
    }

    void EditorAssetManager::ImportTexture(const std::weak_ptr<TextureImporter> importerReference, const AssetLoadCallback& Callback)
    {
        threadPool.PushTask([this, importerReference, Callback]() -> void
        {
            std::shared_ptr<TextureImporter> importer;
            if (importer = importerReference.lock(); importer == nullptr)
            {
                Callback(defaultCheckeredTextureID);
                return;
            }

            const std::optional<ImportedTexture> importedTexture = importer->Import();
            if (!importedTexture.has_value())
            {
                Callback(defaultCheckeredTextureID);
                return;
            }

            const TextureID ID = importedTexture.value().hash;
            TextureQueueEntry textureEntry
            {
                .ID = ID,
                .texture = Texture({
                    .name = importedTexture.value().name.data(),
                    .renderingContext = renderingContext,
                    .width = importedTexture.value().width,
                    .height = importedTexture.value().height,
                    .imageType = importedTexture.value().imageType,
                    .preferredFormat = importedTexture.value().format,
                    .preferredFilter = importedTexture.value().filter,
                    .levelCount = importedTexture.value().levelCount,
                    .layerCount = importedTexture.value().layerCount
                }),
                .levelBuffers = std::vector<std::unique_ptr<Sierra::Buffer>>(importedTexture->levelCount)
            };

            for (size i = 0; i < importedTexture->levelCount; i++)
            {
                textureEntry.levelBuffers[i] = renderingContext.CreateBuffer({
                   .name = fmt::format("Buffer of Texture [{0}]", importedTexture.value().name.data()),
                   .memorySize = sizeof(uint8) * importedTexture.value().levelMemory[i].size(),
                   .usage = Sierra::BufferUsage::SourceMemory,
                   .memoryLocation = Sierra::BufferMemoryLocation::CPU
                });
                textureEntry.levelBuffers[i]->CopyFromMemory(importedTexture.value().levelMemory[i].data());
            }

            {
                const std::lock_guard textureQueueLock(textureQueueMutex);
                textureQueue.emplace(std::move(textureEntry));
            }

            Callback(ID);
        });

    }

    /* --- GETTER METHODS --- */

    const Texture& EditorAssetManager::GetDefaultTexture(const TextureType textureType)
    {
        switch (textureType)
        {
            case TextureType::Undefined:
            case TextureType::Albedo:
            case TextureType::Environment:      return textureMap.find(defaultCheckeredTextureID)->second;
            case TextureType::Specular:
            case TextureType::Opacity:
            case TextureType::Roughness:
            case TextureType::Metallic:
            case TextureType::Displacement:
            case TextureType::Emission:
            case TextureType::Occlusion:
            case TextureType::Shadow:           return textureMap.find(defaultBlackTextureID)->second;
            case TextureType::Normal:           return textureMap.find(defaultNormalTextureID)->second;
        }

        return textureMap.begin()->second;
    }

}