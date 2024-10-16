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

    void Buffer::Write(const void* memory, const size sourceOffset, const size destinationOffset, const size memorySize)
    {
        SR_THROW_IF(memory == nullptr, InvalidValueError(SR_FORMAT("Cannot write memory range [{0}-{1}] to buffer [{1}], as specified memory pointer must not be null", destinationOffset, destinationOffset + memorySize, GetName())));
        SR_THROW_IF(destinationOffset + memorySize > GetMemorySize(), InvalidRangeError(SR_FORMAT("Cannot write invalid memory range to buffer [{0}]", GetName()), destinationOffset, memorySize, size(0), GetMemorySize()));
    }

}