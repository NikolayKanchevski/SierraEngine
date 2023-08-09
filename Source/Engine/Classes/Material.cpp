//
// Created by Nikolay Kanchevski on 6.08.23.
//

#include "Material.h"

#include "../Handlers/Assets/AssetManager.h"

namespace Sierra::Engine
{

    /* --- CONSTRUCTORS --- */

    Material::Material()
    {
        // Use default textures
        textures.resize(static_cast<uint32>(TextureType::TOTAL_COUNT));
        for (uint i = static_cast<uint32>(TextureType::TOTAL_COUNT); i--;)
        {
            textures[i] = AssetManager::GetDefaultCollection().GetTexture(static_cast<TextureType>(i));
        }
    }

    SharedPtr<Material> Material::Create()
    {
        return std::make_shared<Material>();
    }

    /* --- SETTER METHODS --- */

    void Material::SetTexture(const AssetID &assetID, const TextureType textureType)
    {
        ASSERT_ERROR_IF(textureType == TextureType::UNDEFINED, "In order to bind texture to mesh its texture type must be specified and be different from TextureType::NONE");
        textures[static_cast<uint32>(textureType)] = AssetManager::GetTextureCollection().GetResource(assetID);
        texturePresence.SetBit(static_cast<uint32>(textureType), 1);
    }

    void Material::ResetTexture(const TextureType textureType)
    {
        ASSERT_ERROR_IF(textureType == TextureType::UNDEFINED, "In order to reset a mesh's texture the texture type must not be TextureType::UNDEFINED_TEXTURE");
        textures[static_cast<uint32>(textureType)] = AssetManager::GetDefaultCollection().GetTexture(textureType);
        texturePresence.SetBit(static_cast<uint32>(textureType), 0);
    }

}