//
// Created by Nikolay Kanchevski on 5.12.23.
//

#include "Buffer.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Buffer::Buffer(const BufferCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create buffer, as specified name must not be empty"));
        SR_THROW_IF(createInfo.memorySize == 0, InvalidValueError(SR_FORMAT("Memory size of buffer [{0}] must not be equal to [0]", createInfo.name)));
        SR_THROW_IF(createInfo.usage == BufferUsage::Undefined, InvalidValueError(SR_FORMAT("Usage of buffer [{0}] must not be [BufferUsage::Undefined]", createInfo.name)));
        SR_THROW_IF(createInfo.usage & BufferUsage::Uniform && createInfo.usage & BufferUsage::Storage, InvalidConfigurationError(SR_FORMAT("Usage of buffer [{0}] may only contain either [BufferUsage::Uniform] or [BufferUsage::Storage], but never both", createInfo.name)));
    }

    /* --- POLLING METHODS --- */

    bool Buffer::Write(const uint64 destinationOffset, const std::span<const uint8> buffer)
    {
        if (destinationOffset + buffer.size() > GetMemorySize())
            return false;

        return true;
    }

}