//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "../Core/ResourcePool.hpp"

#include "TextureAsset.h"

namespace SierraEngine
{

    struct AssetManagerCreateInfo
    {
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });
    };

    class AssetManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using AssetLoadCallback = std::function<void(AssetID)>;

        /* --- POLLING METHODS --- */
        virtual void Update(std::unique_ptr<Sierra::CommandBuffer> &commandBuffer) = 0;

        [[nodiscard]] virtual bool SerializeTexture(const std::filesystem::path &filePath, const TextureSerializeInfo &serializeInfo) = 0;
        virtual void ImportTexture(const std::filesystem::path &filePath, AssetLoadCallback LoadCallback) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Sierra::Version GetVersion() const { return version; }

        [[nodiscard]] virtual AssetID GetDefaultTexture(TextureType textureType) const = 0;
        [[nodiscard]] virtual std::optional<std::reference_wrapper<const TextureAsset>> GetTexture(AssetID ID) const = 0;

        /* --- OPERATORS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager &operator=(const AssetManager&) = delete;

        /* --- DESTRUCTOR --- */
        ~AssetManager() = default;

    protected:
        explicit AssetManager(const AssetManagerCreateInfo &createInfo);

        template<typename T> requires (std::is_base_of_v<Asset, T> && !std::is_same_v<Asset, T>)
        struct AssetQueueEntry
        {
            AssetID ID;
            T asset;
            std::unique_ptr<Sierra::Buffer> buffer;
            std::function<void(AssetID)> LoadCallback;
        };

        constexpr static char TEXTURE_ASSET_EXTENSION[] = ".texture";
        constexpr static uint16 MAX_TEXTURE_DIMENSIONS = 4096;
        constexpr static uint8 TEXTURE_TYPE_COUNT = static_cast<uint8>(TextureType::Height) + 1;

    private:
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });

    };

}
