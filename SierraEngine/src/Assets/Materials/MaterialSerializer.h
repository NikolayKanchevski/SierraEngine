//
// Created by Nikolay Kanchevski on 19.11.24.
//

#pragma once

#include "../AssetSerializer.h"

#include "../AssetID.h"
#include "../AssetMetadata.h"

namespace SierraEngine
{

    enum class AlphaMode : bool
    {
        Opaque,
        Transparent
    };

    enum class CullMode : bool
    {
        SingleSided,
        DoubleSided
    };

    struct MaterialDiffuse
    {
        Color<3, float32> tint = { 1.0f, 1.0f, 1.0f };
    };

    struct MaterialSpecular
    {
        float32 shininess = 1.0f;
    };

    struct MaterialNormal
    {

    };

    struct MaterialSettings
    {
        MaterialDiffuse diffuse = { };
        TextureID diffuseTexture = { };

        MaterialSpecular specular = { };
        TextureID specularTexture = { };

        MaterialNormal normal = { };
        TextureID normalTexture = { };

        AlphaMode alphaMode = AlphaMode::Opaque;
        CullMode cullMode = CullMode::SingleSided;
    };

    struct MaterialSerializeInfo
    {
        AssetMetadata metadata = { };
        MaterialSettings settings = { };
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
        [[nodiscard]] virtual std::optional<SerializedMaterial> Serialize(const MaterialSerializeInfo& serializeInfo, MaterialID& outID) const = 0;

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
