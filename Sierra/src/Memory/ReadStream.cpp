//
// Created by Nikolay Kanchevski on 24.12.24.
//

#include "ReadStream.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    ReadStream::ReadStream(const std::span<const uint8> buffer)
        : buffer(buffer)
    {
        SR_THROW_IF(buffer.data() == nullptr, InvalidValueError("Cannot create buffer read stream, as specified buffer must not be null"));
    }

    /* --- POLLING METHODS --- */

    StreamResult ReadStream::Read(std::span<uint8> outMemory)
    {
        if (const StreamResult result = Stream::Read(outMemory); result != StreamResult::Success)
            return result;

        std::memcpy(outMemory.data(), outMemory.data() + offset, outMemory.size());
        offset += outMemory.size();

        return StreamResult::Success;
    }

    StreamResult ReadStream::Write(const std::span<const uint8> memory)
    {
        return StreamResult::InvalidAccess;
    }

    /* --- SETTER METHODS --- */

    StreamResult ReadStream::SetOffset(const size givenOffset)
    {
        if (const StreamResult result = Stream::SetOffset(offset); result != StreamResult::Success)
            return result;

        offset = givenOffset;
        return StreamResult::Success;
    }

}