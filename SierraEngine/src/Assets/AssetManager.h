//
// Created by Nikolay Kanchevski on 3.07.24.
//

#pragma once

#include "AssetID.h"

#include "Textures/TextureImporter.h"
#include "Materials/MaterialImporter.h"
#include "../Rendering/RenderingContext.h"

namespace SierraEngine
{

    struct AssetManagerCreateInfo
    {
        const RenderingContext& renderingContext;
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

        /* --- COPY SEMANTICS --- */
        AssetManager(const AssetManager&) = delete;
        AssetManager& operator=(const AssetManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~AssetManager() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit AssetManager(const AssetManagerCreateInfo& createInfo) noexcept;

        /* --- MOVE SEMANTICS --- */
        AssetManager(AssetManager&&) noexcept = default;
        AssetManager& operator=(AssetManager&&) noexcept = default;

    };

}
