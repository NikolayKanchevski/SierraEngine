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
        SR_THROW_IF(GetOffset() + memorySize > GetSize(), InvalidRangeError("Cannot read invalid range from stream", offset, memorySize, static_cast<size>(0), GetSize()));

        std::vector<uint8> readMemory = { };
        readMemory.resize(memorySize);

        std::memcpy(readMemory.data(), memory.data() + offset, memorySize);
        offset += memorySize;

        return readMemory;
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