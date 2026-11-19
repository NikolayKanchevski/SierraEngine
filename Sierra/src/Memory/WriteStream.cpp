//
// Created by Nikolay Kanchevski on 24.12.24.
//

#include "WriteStream.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WriteStream::WriteStream(const size capacity)
    {
        buffer.reserve(capacity);
    }

    /* --- POLLING METHODS --- */

    StreamResult WriteStream::Read(std::span<uint8> outMemory)
    {
        if (const StreamResult result = Stream::Read(outMemory); result != StreamResult::Success)
            return result;

        offset += outMemory.size();
        std::memcpy(outMemory.data(), &buffer[offset - outMemory.size()], outMemory.size());

        return StreamResult::Success;
    }

    StreamResult WriteStream::Write(const std::span<const uint8> givenMemory)
    {
        if (const StreamResult result = Stream::Write(givenMemory); result != StreamResult::Success)
            return result;

        if (const size requiredMemorySize = offset + givenMemory.size(); requiredMemorySize >= buffer.size())
        {
            buffer.resize(requiredMemorySize);
        }

        std::memcpy(buffer.data() + offset, givenMemory.data(), givenMemory.size());
        offset += givenMemory.size();

        return StreamResult::Success;
    }

    void WriteStream::Resize(const size memorySize) noexcept
    {
        buffer.resize(memorySize);
    }

    std::vector<uint8> WriteStream::Release() noexcept
    {
        std::vector<uint8> result = std::move(buffer);

        offset = 0;
        buffer = { };

        return result;
    }

    /* --- SETTER METHODS --- */

    StreamResult WriteStream::SetOffset(const size givenOffset)
    {
        if (const StreamResult result = Stream::SetOffset(givenOffset); result != StreamResult::Success)
            return result;

        offset = givenOffset;
        return StreamResult::Success;
    }

}