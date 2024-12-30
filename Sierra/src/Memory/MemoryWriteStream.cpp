//
// Created by Nikolay Kanchevski on 24.12.24.
//

#include "MemoryWriteStream.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MemoryWriteStream::MemoryWriteStream(const size capacity)
    {
        memory.reserve(capacity);
    }

    /* --- POLLING METHODS --- */

    std::vector<uint8> MemoryWriteStream::Read(const size memorySize)
    {
        SR_THROW_IF(GetOffset() + memorySize > GetSize(), InvalidRangeError("Cannot read invalid range from stream", offset, memorySize, size(0), GetSize()));

        offset += memorySize;
        return { &memory[offset - memorySize], &memory[offset] };
    }

    void MemoryWriteStream::Write(const void* givenMemory, const size memorySize)
    {
        Stream::Write(givenMemory, memorySize);

        if (const size requiredMemorySize = offset + memorySize; requiredMemorySize >= memory.size())
        {
            memory.resize(requiredMemorySize);
        }

        std::memcpy(&memory[offset], givenMemory, memorySize);
        offset += memorySize;
    }

    std::vector<uint8> MemoryWriteStream::Release() noexcept
    {
        offset = 0;
        return std::move(memory);
    }

    /* --- SETTER METHODS --- */

    void MemoryWriteStream::SetOffset(const size givenOffset)
    {
        offset = givenOffset;
    }

}