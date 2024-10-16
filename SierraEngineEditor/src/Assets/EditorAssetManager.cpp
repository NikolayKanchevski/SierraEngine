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
        : AssetManager(), threadPool(createInfo.threadPool), device(createInfo.device), textureMap(3)
    {
        // Default checkered texture
        {
            std::unique_ptr<Sierra::Buffer> textureBuffer = device.CreateBuffer({
                .name = "Buffer of Texture [Default Checkered]",
                .memorySize = sizeof(uint8) * CHECKERED_TEXTURE_MEMORY.size(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            textureBuffer->Write(CHECKERED_TEXTURE_MEMORY.data(), 0, 0, textureBuffer->GetMemorySize());

            defaultCheckeredTextureID = std::hash<std::string_view>{}("$DEFAULT{CHECKERED_TEXTURE}");
            TextureQueueEntry textureEntry
            {
                .ID = defaultCheckeredTextureID,
                .texture = Texture({
                    .name = "Default Checkered Texture",
                    .device = device,
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
            std::unique_ptr<Sierra::Buffer> textureBuffer = device.CreateBuffer({
                .name = "Buffer of Texture [Default Black]",
                .memorySize = sizeof(uint8) * BLACK_TEXTURE_MEMORY.size(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            textureBuffer->Write(BLACK_TEXTURE_MEMORY.data(), 0, 0, textureBuffer->GetMemorySize());

            defaultBlackTextureID = std::hash<std::string_view>{}("$DEFAULT{BLACK_TEXTURE}");
            TextureQueueEntry textureEntry
            {
                .ID = defaultBlackTextureID,
                .texture = Texture({
                    .name = "Default Black Texture",
                    .device = device,
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
            std::unique_ptr<Sierra::Buffer> textureBuffer = device.CreateBuffer({
                .name = "Buffer of Texture [Default Normal]",
                .memorySize = sizeof(uint8) * NORMAL_TEXTURE_MEMORY.size(),
                .usage = Sierra::BufferUsage::SourceMemory,
                .memoryLocation = Sierra::BufferMemoryLocation::CPU
            });
            textureBuffer->Write(NORMAL_TEXTURE_MEMORY.data(), 0, 0, textureBuffer->GetMemorySize());

            defaultNormalTextureID = std::hash<std::string_view>{}("$DEFAULT{NORMAL_TEXTURE}");
            TextureQueueEntry textureEntry
            {
                .ID = defaultNormalTextureID,
                .texture = Texture({
                    .name = "Default Normal Texture",
                    .device = device,
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

            commandBuffer.SynchronizeImageUsage(textureImage, Sierra::ImageCommandUsage::None, Sierra::ImageCommandUsage::MemoryWrite, 0, textureImage.GetLevelCount(), 0, textureImage.GetLayerCount());
            for (uint32 level = 0; level < textureEntry.texture.GetLevelCount(); level++)
            {
                std::unique_ptr<Sierra::Buffer>& levelBuffer = textureEntry.levelBuffers[level];
                for (uint32 layer = 0; layer < textureEntry.texture.GetLayerCount(); layer++)
                {
                    commandBuffer.CopyBufferToImage(*levelBuffer, textureImage, level, layer, layer * (levelBuffer->GetMemorySize() / textureImage.GetLayerCount()), { 0, 0, 0 }, { textureImage.GetWidth() >> level, textureImage.GetHeight() >> level, textureImage.GetDepth() >> level });
                }
                commandBuffer.QueueBufferForDestruction(std::move(levelBuffer));
            }
            commandBuffer.SynchronizeImageUsage(textureImage, Sierra::ImageCommandUsage::MemoryWrite, Sierra::ImageCommandUsage::GraphicsRead, 0, textureImage.GetLevelCount(), 0, textureImage.GetLayerCount());

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
                    .device = device,
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
                textureEntry.levelBuffers[i] = device.CreateBuffer({
                   .name = SR_FORMAT("Buffer of Texture [{0}]", importedTexture.value().name.data()),
                   .memorySize = sizeof(uint8) * importedTexture.value().levelMemory[i].size(),
                   .usage = Sierra::BufferUsage::SourceMemory,
                   .memoryLocation = Sierra::BufferMemoryLocation::CPU
                });
                textureEntry.levelBuffers[i]->Write(importedTexture.value().levelMemory[i].data(), 0, 0, textureEntry.levelBuffers[i]->GetMemorySize());
            }

            {
                const std::lock_guard textureQueueLock(textureQueueMutex);
                textureQueue.emplace(std::move(textureEntry));
            }

            Callback(ID);
        });

    }

    /* --- GETTER METHODS --- */

    const Texture& EditorAssetManager::GetDefaultTexture(const TextureType textureType) const noexcept
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
