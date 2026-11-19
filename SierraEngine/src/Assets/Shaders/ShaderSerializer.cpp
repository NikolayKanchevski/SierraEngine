//
// Created by Nikolay Kanchevski on 18.11.26.
//

#include "ShaderSerializer.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] Sierra::StreamResult WriteRange(Sierra::Stream& blob, const std::span<const uint8> buffer, const size offset, const size memorySize)
        {
            if (offset > buffer.size() || memorySize > buffer.size() - offset)
                return Sierra::StreamResult::InvalidRange;

            return blob.Write(buffer.subspan(offset, memorySize));
        }
    }

    /* --- POLLING METHODS --- */

    Sierra::StreamResult ShaderSerializer::SerializeBlob(Sierra::Stream& blob, const ShaderSerializeInfo& serializeInfo) const
    {
        for (const ShaderEntryPoint& entryPoint : serializeInfo.entryPoints)
        {
            if (const Sierra::StreamResult result = WriteRange(blob, serializeInfo.buffer, entryPoint.spirvCodeOffset, entryPoint.spirvCodeMemorySize); result != Sierra::StreamResult::Success)
                return result;

            if (const Sierra::StreamResult result = WriteRange(blob, serializeInfo.buffer, entryPoint.macOSMetalCodeOffset, entryPoint.macOSMetalCodeMemorySize); result != Sierra::StreamResult::Success)
                return result;

            if (const Sierra::StreamResult result = WriteRange(blob, serializeInfo.buffer, entryPoint.iOSMetalCodeOffset, entryPoint.iOSMetalCodeMemorySize); result != Sierra::StreamResult::Success)
                return result;
        }

        return Sierra::StreamResult::Success;
    }

}
