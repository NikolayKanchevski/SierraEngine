//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "AssetID.h"

#include "Textures/TextureImporter.h"
#include "Materials/MaterialImporter.h"

namespace SierraEngine
{

    struct AssetManagerCreateInfo
    {
        const Sierra::Device& device;
    };

    class SIERRA_ENGINE_API AssetManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        template<AssetIDType IDType>
        using AssetLoadCallback = std::function<void(IDType)>;

        /* --- POLLING METHODS --- */
        virtual void Update(Sierra::CommandBuffer& commandBuffer) = 0;

        virtual void ImportTexture(const ImportedTexture& importedTexture) = 0;
        virtual void ImportMaterial(const ImportedMaterial& importedMaterial) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const TextureAsset& GetDefaultTexture(TextureType textureType) const noexcept = 0;
        [[nodiscard]] virtual const MaterialAsset& GetDefaultMaterial() const noexcept = 0;

        [[nodiscard]] bool TextureExists(TextureID ID) const noexcept { return GetTexture(ID) != nullptr; }
        [[nodiscard]] virtual const TextureAsset* GetTexture(TextureID ID) const noexcept = 0;

        [[nodiscard]] bool MaterialExists(MaterialID ID) const noexcept { return GetMaterial(ID) != nullptr; }
        [[nodiscard]] virtual const MaterialAsset* GetMaterial(MaterialID ID) const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        AssetManager(AssetManager&&) = delete;
        AssetManager& operator=(AssetManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetManager() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit AssetManager(const AssetManagerCreateInfo& createInfo) noexcept;

    };

}
