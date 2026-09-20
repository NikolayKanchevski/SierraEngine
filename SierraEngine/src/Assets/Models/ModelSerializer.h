//
// Created by Nikolay Kanchevski on 30.12.24.
//

#pragma once

#include "../AssetSerializer.h"

#include "ModelLoader.h"
#include "../AssetMetadata.h"

#include "../Textures/TextureSerializer.h"
#include "../Materials/MaterialSerializer.h"

namespace SierraEngine
{

    struct ModelSettings
    {

    };

    enum class MeshCompression : uint8
    {
        None
    };

    struct MeshCompressionSettings
    {
        MeshCompression compression = MeshCompression::None;
    };

    struct ModelSerializeInfo
    {
        AssetMetadata metadata = { };
        ModelSettings settings = { };

        TextureCompressionSettings textureCompressionSettings = { };
        MeshCompressionSettings meshCompressionSettings = { };

        const LoadedModel& model;
    };

    struct SerializedModel
    {
        std::vector<uint8> data = { };
        std::vector<uint8> blob = { };
    };

    struct SerializedModelTextureDependency
    {
        TextureID ID = { };
        std::string name = { };
        SerializedTexture texture = { };
    };

    struct SerializedModelMaterialDependency
    {
        MaterialID ID = { };
        std::string name = { };
        SerializedMaterial material = { };
    };

    struct SerializedModelDependencies
    {
        std::vector<std::optional<SerializedModelTextureDependency>> textures = { };
        std::vector<std::optional<SerializedModelMaterialDependency>> materials = { };
    };

    class SIERRA_ENGINE_API ModelSerializer : public virtual AssetSerializer
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<SerializedModel> Serialize(const ModelSerializeInfo& serializeInfo, SerializedModelDependencies& outDependencies, ModelID& outID) const = 0;

        /* --- COPY SEMANTICS --- */
        ModelSerializer(const ModelSerializer&) = delete;
        ModelSerializer& operator=(const ModelSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~ModelSerializer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ModelSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        void SerializeBlob(Sierra::Stream& blob, const ModelSerializeInfo& serializeInfo) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetModelMemorySize(const ModelSerializeInfo& serializeInfo) const noexcept;

        /* --- MOVE SEMANTICS --- */
        ModelSerializer(ModelSerializer&&) noexcept = default;
        ModelSerializer& operator=(ModelSerializer&&) noexcept = default;

    };

}