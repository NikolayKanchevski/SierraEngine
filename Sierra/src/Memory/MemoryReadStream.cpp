//
// Created by Nikolay Kanchevski on 24.12.24.
//

#include "MemoryReadStream.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MemoryReadStream::MemoryReadStream(const std::span<const uint8> memory)
        : memory(memory)
    {
        SR_THROW_IF(memory.data() == nullptr, InvalidValueError("Cannot create memory read stream, as specified memory must not be null"));
    }

    /* --- POLLING METHODS --- */

    void MemoryReadStream::Read(std::span<uint8> outBuffer)
    {
        SR_THROW_IF(GetOffset() + outBuffer.size() > GetSize(), InvalidRangeError("Cannot read invalid range from stream", offset, outBuffer.size(), static_cast<size>(0), GetSize()));

        std::memcpy(outBuffer.data(), memory.data() + offset, outBuffer.size());
        offset += outBuffer.size();
    }

    void MemoryReadStream::Write(const void* givenMemory, const size memorySize)
    {
        Stream::Write(givenMemory, memorySize);
    }

    /* --- SETTER METHODS --- */

    void MemoryReadStream::SetOffset(const size givenOffset)
    {
        offset = givenOffset;
    }

}