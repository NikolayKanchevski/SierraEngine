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

    std::vector<uint8> MemoryReadStream::Read(const size memorySize)
    {
        SR_THROW_IF(GetOffset() + memorySize > GetSize(), InvalidRangeError("Cannot read invalid range from stream", offset, memorySize, size(0), GetSize()));

        offset += memorySize;
        return std::vector<uint8>(&memory[offset - memorySize], &memory[offset] );
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