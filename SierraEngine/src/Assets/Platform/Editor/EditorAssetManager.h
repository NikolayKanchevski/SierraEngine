//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "../../AssetManager.h"

#include "../../../Core/ThreadPool.hpp"
#include "TextureSerializer.h"
#include "../../TextureImporter.h"

namespace SierraEngine
{

    class EditorAssetManager final : public AssetManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        EditorAssetManager(ThreadPool &threadPool, const Sierra::RenderingContext &renderingContext, const AssetManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer) override;

        [[nodiscard]] bool SerializeTexture(const std::filesystem::path &filePath, const TextureSerializeInfo &serializeInfo) override;
        void ImportTexture(const std::filesystem::path &filePath, AssetLoadCallback LoadCallback) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline AssetID GetDefaultTexture(const TextureType textureType) const override { return defaultTextures[static_cast<uint8>(textureType)]; }
        [[nodiscard]] inline std::optional<std::reference_wrapper<const TextureAsset>> GetTexture(const AssetID ID) const override { return texturePool.GetResource(ID); }

        /* --- DESTRUCTOR --- */
        ~EditorAssetManager();

    private:
        ThreadPool &threadPool;
        const Sierra::RenderingContext &renderingContext;

        TextureSerializer textureSerializer;
        TextureImporter textureImporter;
        ResourcePool<AssetID, TextureAsset> texturePool;
        std::array<AssetID, TEXTURE_TYPE_COUNT> defaultTextures;

        std::mutex textureQueueMutex;
        std::queue<AssetQueueEntry<TextureAsset>> textureQueue;

    };

}
