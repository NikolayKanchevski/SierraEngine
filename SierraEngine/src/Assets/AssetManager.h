//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "../Core/ThreadPool.hpp"
#include "../Core/ResourcePool.hpp"

#include "AssetID.hpp"

#include "../Rendering/Texture.h"
#include "Importers/TextureImporter.h"

namespace SierraEngine
{

    struct AssetManagerCreateInfo
    {
        ThreadPool &threadPool;
        const Sierra::FileManager &fileManager;
        const Sierra::RenderingContext &renderingContext;
    };

    class AssetManager final
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using AssetLoadCallback = std::function<void(AssetID)>;

        /* --- CONSTRUCTORS --- */
        explicit AssetManager(const AssetManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer);

        void ImportTexture(const std::filesystem::path &assetFilePath, const AssetLoadCallback &LoadCallback);

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetID GetDefaultTexture(TextureType textureType) const;
        [[nodiscard]] std::optional<std::reference_wrapper<const Texture>> GetTexture(AssetID ID) const;

        /* --- OPERATORS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        /* --- DESTRUCTOR --- */
        ~AssetManager() = default;

    private:
        template<typename A, typename I, uint32 C>
        struct AssetBackend
        {
            struct AssetQueueEntry
            {
                AssetID ID;
                I importedAsset;
                std::function<void(AssetID)> LoadCallback;
            };

            std::mutex queueMutex = { };
            std::queue<AssetQueueEntry> queue = { };

            ResourcePool<AssetID, A, C> pool = { };
        };

    private:
        ThreadPool &threadPool;
        const Sierra::FileManager &fileManager;
        const Sierra::RenderingContext &renderingContext;

        AssetBackend<Texture, ImportedTexture, 256> textureBackend = { };
        AssetID defaultCheckeredTexture;
        AssetID defaultBlackTexture;
        AssetID defaultNormalTexture;

    };

}
