//
// Created by Nikolay Kanchevski on 30.12.24.
//

#include "ModelSerializer.h"

namespace SierraEngine
{

    namespace
    {
        void SerializeRawBlob(Sierra::Stream& stream, const ModelSerializeInfo& serializeInfo)
        {
            stream.Write(serializeInfo.model.vertices.data(), sizeof(Vertex) * serializeInfo.model.vertices.size());
            stream.Write(serializeInfo.model.indices.data(), sizeof(uint32) * serializeInfo.model.indices.size());
        }

        void SerializeCompressedBlob(Sierra::Stream& stream, const ModelSerializeInfo& serializeInfo)
        {
            APP_ERROR("Unimplemented");
        }
    }

    /* --- POLLING METHODS --- */

    void ModelSerializer::SerializeBlob(Sierra::Stream& blob, const ModelSerializeInfo& serializeInfo) const
    {
        if (serializeInfo.meshCompressionSettings.compression == MeshCompression::None)
        {
            SerializeRawBlob(blob, serializeInfo);
            return;
        }

        SerializeCompressedBlob(blob, serializeInfo);
    }

    /* --- GETTER METHODS --- */

    size ModelSerializer::GetModelMemorySize(const ModelSerializeInfo& serializeInfo) const noexcept
    {
        return (sizeof(Vertex) * serializeInfo.model.vertices.size()) + (sizeof(uint32) * serializeInfo.model.indices.size());
    }

}