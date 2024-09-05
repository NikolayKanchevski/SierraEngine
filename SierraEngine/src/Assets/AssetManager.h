//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "Assets/Texture.h"
#include "Importers/TextureImporter.h"

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using AssetID = Sierra::UUID64;
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
        [[nodiscard]] virtual const Texture* GetTexture(TextureID textureID) = 0;
        [[nodiscard]] virtual const Texture& GetDefaultTexture(TextureType textureType) = 0;

        /* --- COPY SEMANTICS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        AssetManager(AssetManager&&) = default;
        AssetManager& operator=(AssetManager&&) = default;

        /* --- DESTRUCTOR --- */
        virtual ~AssetManager() = default;

    protected:
        AssetManager() = default;

    };

}
