//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "AssetID.h"

#include "Assets/Texture.h"
#include "Importers/TextureImporter.h"

namespace SierraEngine
{

    using TextureID = AssetID;

    class SIERRA_ENGINE_API AssetManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using AssetLoadCallback = std::function<void(AssetID)>;

        /* --- POLLING METHODS --- */
        virtual void Update(Sierra::CommandBuffer &commandBuffer) = 0;

        virtual void ImportTexture(std::weak_ptr<TextureImporter> importer, AssetLoadCallback Callback) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsTextureImported(TextureID ID) const = 0;
        [[nodiscard]] virtual Texture* GetTexture(TextureID ID) = 0;
        [[nodiscard]] virtual Texture& GetDefaultTexture(TextureType type) = 0;

        /* --- OPERATORS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetManager() = default;

    protected:
        AssetManager() = default;

    };

}
