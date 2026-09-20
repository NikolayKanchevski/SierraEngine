//
// Created by Nikolay Kanchevski on 12.08.25.
//

#include "ModelImporter.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] std::optional<ImportedModelBuffer> ImportRawBlob(Sierra::Stream& blobStream, const ModelProperties& properties) noexcept
        {
            ImportedModelBuffer buffer = { };

            buffer.vertices.resize(properties.totalVertexCount);
            {
                const std::vector<uint8> vertexMemory = blobStream.Read(properties.totalVertexCount * sizeof(Vertex));
                std::memcpy(buffer.vertices.data(), vertexMemory.data(), vertexMemory.size());
            }

            // TODO: Stream rework + direct pointer copy
            buffer.indices.resize(properties.totalIndexCount);
            {
                const std::vector<uint8> indexMemory = blobStream.Read(properties.totalIndexCount * sizeof(uint32));
                std::memcpy(buffer.indices.data(), indexMemory.data(), indexMemory.size());
            }

            return buffer;
        }

        [[nodiscard]] std::optional<ImportedModelBuffer> ImportCompressedBlob(Sierra::Stream& blobStream, const ModelProperties& properties) noexcept
        {
            APP_ERROR("Unimplemented");
            return { };
        }
    }

    /* --- POLLING METHODS --- */

    std::optional<ImportedModelBuffer> ModelImporter::ImportBlob(Sierra::Stream& blobStream, const ModelProperties& properties) const
    {
        if (properties.compression == MeshCompression::None)
            return ImportRawBlob(blobStream, properties);

        return ImportCompressedBlob(blobStream, properties);
    }

}
