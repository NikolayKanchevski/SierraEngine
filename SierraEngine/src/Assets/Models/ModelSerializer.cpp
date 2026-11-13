//
// Created by Nikolay Kanchevski on 30.12.24.
//

#include "ModelSerializer.h"

namespace SierraEngine
{

    namespace
    {
        void SerializeRawBlob(Sierra::Stream& stream, const std::span<const Vertex> vertices, const std::span<const uint32> indices)
        {
            stream.Write(vertices.data(), sizeof(Vertex) * vertices.size());
            stream.Write(indices.data(), sizeof(uint32) * indices.size());
        }

        void SerializeCompressedBlob(Sierra::Stream& stream, const std::span<const Vertex> vertices, const std::span<const uint32> indices)
        {
            APP_ERROR("Unimplemented");
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<ModelGeometry> ModelSerializer::OptimizeGeometry(const ModelSerializeInfo& serializeInfo) const
    {
        if (serializeInfo.meshOptimizer == nullptr) return std::nullopt;

        const std::span<const Vertex> vertices = serializeInfo.model.vertices;
        const std::span<const uint32> indices = serializeInfo.model.indices;

        ModelGeometry geometry = { };
        geometry.vertices.reserve(vertices.size());
        geometry.indices.reserve(indices.size());
        geometry.meshes.reserve(serializeInfo.model.meshes.size());

        for (const ModelMesh& mesh : serializeInfo.model.meshes)
        {
            if (static_cast<size>(mesh.vertexOffset) + mesh.vertexCount > vertices.size() || static_cast<size>(mesh.indexOffset) + mesh.indexCount > indices.size())
            {
                APP_WARNING("Could not optimize geometry of model [{0}], as one of its meshes exceeds the bounds of the geometry buffers, writing unoptimized geometry instead", serializeInfo.metadata.name);
                return std::nullopt;
            }

            const std::span<const Vertex> meshVertices = vertices.subspan(mesh.vertexOffset, mesh.vertexCount);
            const std::span<const uint32> meshIndices = indices.subspan(mesh.indexOffset, mesh.indexCount);

            ModelMesh& optimizedMesh = geometry.meshes.emplace_back(mesh);
            optimizedMesh.vertexOffset = static_cast<uint32>(geometry.vertices.size());
            optimizedMesh.indexOffset = static_cast<uint32>(geometry.indices.size());

            // Indices are local to their mesh, so every mesh can be optimized on its own
            const std::optional<Mesh> result = serializeInfo.meshOptimizer->Optimize({ .vertices = meshVertices, .indices = meshIndices });
            if (!result.has_value())
            {
                APP_WARNING("Could not optimize mesh [{0}] of model [{1}], writing its geometry unoptimized instead", geometry.meshes.size() - 1, serializeInfo.metadata.name);
                geometry.vertices.insert(geometry.vertices.end(), meshVertices.begin(), meshVertices.end());
                geometry.indices.insert(geometry.indices.end(), meshIndices.begin(), meshIndices.end());
                continue;
            }

            optimizedMesh.vertexCount = static_cast<uint32>(result->vertices.size());
            optimizedMesh.indexCount = static_cast<uint32>(result->indices.size());

            geometry.vertices.insert(geometry.vertices.end(), result->vertices.begin(), result->vertices.end());
            geometry.indices.insert(geometry.indices.end(), result->indices.begin(), result->indices.end());
        }

        return geometry;
    }

    void ModelSerializer::SerializeBlob(Sierra::Stream& blob, const ModelSerializeInfo& serializeInfo, const std::span<const Vertex> vertices, const std::span<const uint32> indices) const
    {
        if (serializeInfo.meshCompressionSettings.compression == MeshCompression::None)
        {
            SerializeRawBlob(blob, vertices, indices);
            return;
        }

        SerializeCompressedBlob(blob, vertices, indices);
    }

    /* --- GETTER METHODS --- */

    size ModelSerializer::GetModelMemorySize(const std::span<const Vertex> vertices, const std::span<const uint32> indices) const noexcept
    {
        return (sizeof(Vertex) * vertices.size()) + (sizeof(uint32) * indices.size());
    }

}
