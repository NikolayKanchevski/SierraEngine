//
// Created by Nikolay Kanchevski on 19.11.24.
//

#pragma once

#include "../AssetSerializer.h"

#include "../AssetID.h"
#include "../AssetMetadata.h"

namespace SierraEngine
{

    enum class MaterialAlphaMode : bool
    {
        Opaque,
        Transparent
    };

    enum class MaterialCullMode : bool
    {
        SingleSided,
        DoubleSided
    };

    struct MaterialDiffuseProperty
    {
        Color<3, float32> tint = { 1.0f, 1.0f, 1.0f };
        TextureID texture = { };
    };

    struct MaterialSpecularProperty
    {
        float32 shininess = 1.0f;
        TextureID texture = { };
    };

    struct MaterialNormalProperty
    {
        TextureID texture = { };
    };

    struct MaterialProperties
    {
        MaterialDiffuseProperty diffuse = { };
        MaterialSpecularProperty specular = { };
        MaterialNormalProperty normal = { };

        MaterialAlphaMode alphaMode = MaterialAlphaMode::Opaque;
        MaterialCullMode cullMode = MaterialCullMode::SingleSided;
    };

    struct MaterialSerializeInfo
    {
        AssetMetadata metadata = { };
        MaterialProperties properties = { };
    };

    struct SerializedMaterial
    {
        std::vector<uint8> data = { };
        std::vector<uint8> blob = { };
    };

    class SIERRA_ENGINE_API MaterialSerializer : public virtual AssetSerializer
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<SerializedMaterial> Serialize(const MaterialSerializeInfo& serializeInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        MaterialSerializer(const MaterialSerializer&) = delete;
        MaterialSerializer& operator=(const MaterialSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~MaterialSerializer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        MaterialSerializer() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        MaterialSerializer(MaterialSerializer&&) noexcept = default;
        MaterialSerializer& operator=(MaterialSerializer&&) noexcept = default;

    };

}
