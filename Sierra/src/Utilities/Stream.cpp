//
// Created by Nikolay Kanchevski on 24.12.24.
//

#include "Stream.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void Stream::Write(const void* memory, const size memorySize)
    {
        SR_THROW_IF(GetAccess() == StreamAccess::ReadOnly, InvalidOperationError(SR_FORMAT("Cannot write memory range [{0}-{1}] of stream, as it is read-only!", GetOffset(), GetOffset() + memorySize)));
    }

    void Stream::Write(const void* memory, const size destinationOffset, const size memorySize)
    {
        SetOffset(destinationOffset);
        Write(memory, memorySize);
    }

    void Stream::WriteMemory(const std::span<const uint8> memory)
    {
        Write(memory.data(), memory.size());
    }

    void Stream::WriteMemory(std::span<const uint8> memory, const size destinationOffset)
    {
        SetOffset(destinationOffset);
        WriteMemory(memory);
    }

    std::vector<uint8> Stream::Read(const size sourceOffset, const size memorySize)
    {
        SetOffset(sourceOffset);
        return Read(memorySize);
    }

    std::vector<uint8> Stream::Peek(const size memorySize)
    {
        std::vector<uint8> memory = Read(memorySize);
        SetOffset(GetOffset() - memorySize);
        return memory;
    }

    std::vector<uint8> Stream::Peek(const size sourceOffset, const size memorySize)
    {
        SetOffset(sourceOffset);
        return Peek(memorySize);
    }

    std::vector<uint8> Stream::ReadAll()
    {
        SetOffset(0);
        return Read(GetSize());
    }

    std::vector<uint8> Stream::ReadToEnd()
    {
        return Read(GetSize() - GetOffset());
    }

    /* --- SETTER METHODS --- */

    void Stream::SetOffset(const size offset)
    {
        SR_THROW_IF(offset >= GetSize(), InvalidValueError(SR_FORMAT("Cannot seek to invalid offset [{0}] in stream", offset)));

    }

}