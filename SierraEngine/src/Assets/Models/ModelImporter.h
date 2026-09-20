//
// Created by Nikolay Kanchevski on 12.08.25.
//

#pragma once

#include "../AssetImporter.h"

#include "Mesh.h"
#include "ModelSerializer.h"
#include "../Textures/TextureImporter.h"
#include "../Materials/MaterialImporter.h"

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

    struct ImportedModelBuffer
    {
        std::vector<Vertex> vertices = { };
        std::vector<uint32> indices = { };
    };

    struct ImportedModel
    {
        ModelID ID = { };
        AssetHeader header = { };
        AssetMetadata metadata = { };
        ModelSettings settings = { };

        ModelProperties properties = { };
        ImportedModelBuffer buffer = { };
    };

    class SIERRA_ENGINE_API ModelImporter : public virtual AssetImporter
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<ImportedModel> Import(const ModelImportInfo& importInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        ModelImporter(const ModelImporter&) = delete;
        ModelImporter& operator=(const ModelImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~ModelImporter() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ModelImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedModelBuffer> ImportBlob(Sierra::Stream& blobStream, const ModelProperties& properties) const;

        /* --- MOVE SEMANTICS --- */
        ModelImporter(ModelImporter&&) noexcept = default;
        ModelImporter& operator=(ModelImporter&&) noexcept = default;

    };

}