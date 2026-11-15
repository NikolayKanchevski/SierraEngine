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

    void MemoryWriteStream::Read(std::span<uint8> outBuffer)
    {
        SR_THROW_IF(GetOffset() + outBuffer.size() > GetSize(), InvalidRangeError("Cannot read invalid range from stream", offset, outBuffer.size(), size(0), GetSize()));

        offset += outBuffer.size();
        std::memcpy(outBuffer.data(), &memory[offset - outBuffer.size()], outBuffer.size());
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