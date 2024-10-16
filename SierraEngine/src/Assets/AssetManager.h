//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "Assets/Texture.h"
#include "Importers/TextureImporter.h"

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using AssetID = Sierra::Hash64;
    using TextureID = AssetID;

    class SIERRA_ENGINE_API AssetManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using AssetLoadCallback = std::function<void(AssetID)>;

        /* --- POLLING METHODS --- */
        virtual void Update(Sierra::CommandBuffer& commandBuffer) = 0;

        virtual void ImportTexture(std::weak_ptr<TextureImporter> importer, const AssetLoadCallback& Callback) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const Texture* GetTexture(TextureID textureID) const noexcept = 0;
        [[nodiscard]] virtual const Texture& GetDefaultTexture(TextureType textureType) const noexcept = 0;

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
        AssetManager() noexcept = default;

    };

}
