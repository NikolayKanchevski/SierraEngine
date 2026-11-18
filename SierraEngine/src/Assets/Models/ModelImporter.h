//
// Created by Nikolay Kanchevski on 12.08.25.
//

#pragma once

#include "../AssetImporter.h"

#include "ModelSerializer.h"
#include "../Textures/TextureImporter.h"

namespace SierraEngine
{

    struct ModelImportInfo
    {
        const SerializedModel& serializedModel;
    };

    struct ModelProperties
    {
        std::vector<ModelNode> nodes = { };

        std::vector<ModelMesh> meshes = { };
        size totalVertexCount = 0;
        size totalIndexCount = 0;
        MeshCompression compression = MeshCompression::None;

        std::vector<TextureID> textures = { };
        std::vector<MaterialID> materials = { };
    };

    struct ImportedModel
    {
        ModelID ID = { };
        AssetHeader header = { };
        AssetMetadata metadata = { };
        ModelSettings settings = { };

        ModelProperties properties = { };
        std::vector<Vertex> vertexBuffer = { };
        std::vector<uint32> indexBuffer = { };
    };

    class SIERRA_ENGINE_API ModelImporter : public virtual AssetImporter
    {
    public:
        /* --- CONSTANTS METHODS --- */
        constexpr static AssetTypeSignature TYPE_SIGNATURE = { 'M', 'D' };

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<ImportedModel> Import(const ModelImportInfo& importInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetTypeSignature GetTypeSignature() const noexcept override { return TYPE_SIGNATURE; }

        /* --- COPY SEMANTICS --- */
        ModelImporter(const ModelImporter&) = delete;
        ModelImporter& operator=(const ModelImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~ModelImporter() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ModelImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] Sierra::StreamResult ImportBlob(Sierra::Stream& stream, const ModelProperties& properties, std::vector<Vertex>& outVertexBuffer, std::vector<uint32>& outIndexBuffer) const;

        /* --- MOVE SEMANTICS --- */
        ModelImporter(ModelImporter&&) noexcept = default;
        ModelImporter& operator=(ModelImporter&&) noexcept = default;

    };

}