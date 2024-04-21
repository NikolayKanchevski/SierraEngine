//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "../../AssetManager.h"

#include "../../../Core/ResourcePool.hpp"

namespace SierraEngine
{

    class EditorAssetManager final : public AssetManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit EditorAssetManager(const AssetManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer) override;

        bool SerializeTexture(const std::initializer_list<std::initializer_list<std::filesystem::path>> &levelFilePaths, const TextureSerializeInfo &serializeInfo) override;
        void ImportTexture(const std::filesystem::path &assetFilePath, AssetLoadCallback LoadCallback) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetID GetDefaultTexture(TextureType textureType) const override;
        [[nodiscard]] inline std::optional<std::reference_wrapper<const TextureAsset>> GetTexture(const AssetID ID) const override { return texturePool.GetResource(ID); }

        /* --- DESTRUCTOR --- */
        ~EditorAssetManager() override = default;

    private:
        std::mutex textureQueueMutex;
        std::queue<TextureAssetQueueEntry> textureQueue;
        ResourcePool<AssetID, TextureAsset> texturePool;

        AssetID defaultCheckeredTexture;
        AssetID defaultBlackTexture;
        AssetID defaultNormalTexture;

    };

}
