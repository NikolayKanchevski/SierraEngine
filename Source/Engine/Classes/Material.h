//
// Created by Nikolay Kanchevski on 20.07.23.
//

#pragma once

#include "Asset.h"
#include "Binary.h"
#include "../Types.h"
#include "../../Core/Rendering/Abstractions/Texture.h"

namespace Sierra::Engine
{
    class Material
    {
    public:
        /* --- PROPERTIES --- */
        Vector3 diffuse = Vector3(1.0f, 1.0f, 1.0f);
        float specular = 1.0f;
        float shininess = 0.001953125f;
        float vertexExaggeration = 0.0f;

        /* --- CONSTRUCTORS --- */
        Material();
        static SharedPtr<Material> Create();

        /* --- SETTER METHODS --- */
        void SetTexture(const AssetID &assetID, TextureType textureType);
        void ResetTexture(TextureType textureType);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline SharedPtr<Rendering::Texture>& GetTexture(const TextureType textureType) { return textures[static_cast<uint32>(textureType)]; };

    private:
        Binary texturePresence;
        std::vector<SharedPtr<Rendering::Texture>> textures;

        // NOTE: This constructor is only to be called for the default material inside the DefaultCollection class
        inline Material(const std::vector<SharedPtr<Rendering::Texture>> &textures) : textures(std::move(textures)) {  };
        friend class DefaultCollection;

    };
}