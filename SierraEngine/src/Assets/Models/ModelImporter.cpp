//
// Created by Nikolay Kanchevski on 12.08.25.
//

#include "ModelImporter.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] Sierra::StreamResult ImportRawBlob(Sierra::Stream& blob, const ModelProperties& properties, std::vector<Vertex>& outVertexBuffer, std::vector<uint32>& outIndexBuffer) noexcept
        {
            outVertexBuffer.resize(properties.totalVertexCount);
            if (const Sierra::StreamResult result = blob.Read({ reinterpret_cast<uint8*>(outVertexBuffer.data()), outVertexBuffer.size() * sizeof(Vertex) }); result != Sierra::StreamResult::Success)
            {
                return result;
            }

            outIndexBuffer.resize(properties.totalIndexCount);
            if (const Sierra::StreamResult result = blob.Read({ reinterpret_cast<uint8*>(outIndexBuffer.data()), outIndexBuffer.size() * sizeof(uint32) }); result != Sierra::StreamResult::Success)
            {
                return result;
            }

            return Sierra::StreamResult::Success;
        }

        [[nodiscard]] Sierra::StreamResult ImportCompressedBlob(Sierra::Stream& blob, const ModelProperties& properties, std::vector<Vertex>& outVertexBuffer, std::vector<uint32>& outIndexBuffer) noexcept
        {
            APP_ERROR("Unimplemented");
            return { };
        }
    }

    /* --- POLLING METHODS --- */

    Sierra::StreamResult ModelImporter::ImportBlob(Sierra::Stream& blob, const ModelProperties& properties, std::vector<Vertex>& outVertexBuffer, std::vector<uint32>& outIndexBuffer) const
    {
        if (properties.compression == MeshCompression::None)
            return ImportRawBlob(blob, properties, outVertexBuffer, outIndexBuffer);

        return ImportCompressedBlob(blob, properties, outVertexBuffer, outIndexBuffer);
    }

}
