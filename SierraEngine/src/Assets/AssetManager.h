//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "../Core/ThreadPool.hpp"

#include "TextureAsset.h"

namespace SierraEngine
{

    struct AssetManagerCreateInfo
    {
        ThreadPool &threadPool;
        const Sierra::FileManager &fileManager;
        const Sierra::RenderingContext &renderingContext;
    };

    class AssetManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using AssetLoadCallback = std::function<void(AssetID)>;

        /* --- POLLING METHODS --- */
        virtual void Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer) = 0;

        virtual bool SerializeTexture(const std::initializer_list<std::initializer_list<std::filesystem::path>> &levelFilePaths, const TextureSerializeInfo &serializeInfo) = 0;
        virtual void ImportTexture(const std::filesystem::path &assetFilePath, AssetLoadCallback LoadCallback) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual AssetID GetDefaultTexture(TextureType textureType) const = 0;
        [[nodiscard]] virtual std::optional<std::reference_wrapper<const TextureAsset>> GetTexture(AssetID ID) const = 0;

        /* --- OPERATORS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetManager() = default;

    protected:
        explicit AssetManager(const AssetManagerCreateInfo &createInfo);

        [[nodiscard]] inline ThreadPool& GetThreadPool() { return threadPool; }
        [[nodiscard]] inline const Sierra::FileManager& GetFileManager() const { return fileManager; }
        [[nodiscard]] inline const Sierra::RenderingContext& GetRenderingContext() const { return renderingContext; }

        template<typename T, typename U> requires (std::is_base_of_v<Asset, T> && !std::is_same_v<Asset, T>)
        struct AssetQueueEntry
        {
            AssetID ID;
            T asset;
            U data;
            std::function<void(AssetID)> LoadCallback;
        };

        constexpr static std::string_view TEXTURE_ASSET_EXTENSION = ".texture";
        constexpr static uint16 MAX_TEXTURE_DIMENSIONS = 4096;
        using TextureAssetQueueEntry = AssetQueueEntry<TextureAsset, std::vector<std::unique_ptr<Sierra::Buffer>>>;

    private:
        ThreadPool &threadPool;
        const Sierra::FileManager &fileManager;
        const Sierra::RenderingContext &renderingContext;

    };

}
