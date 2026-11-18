//
// Created by Nikolay Kanchevski on 24.12.24.
//

#include "Stream.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    StreamResult Stream::Read(const std::span<uint8> outMemory)
    {
        if (GetAccess() == StreamAccess::WriteOnly)
            return StreamResult::InvalidAccess;

        if (GetOffset() + outMemory.size() > GetSize())
            return StreamResult::InvalidRange;

        return StreamResult::Success;
    }

    StreamResult Stream::Write(std::span<const uint8> memory)
    {
        // NOTE: Unlike reads, writes are allowed to go past the end, as that is how streams grow
        if (GetAccess() == StreamAccess::ReadOnly)
            return StreamResult::InvalidAccess;

        return StreamResult::Success;
    }

    StreamResult Stream::ReadToEnd(std::vector<uint8>& outMemory)
    {
        outMemory.resize(GetSize() - GetOffset());
        return Read(outMemory);
    }

    StreamResult Stream::ReadAll(std::vector<uint8>& outMemory)
    {
        if (const StreamResult result = SetOffset(0); result != StreamResult::Success)
            return result;

        outMemory.resize(GetSize());
        return Read(outMemory);
    }

    /* --- SETTER METHODS --- */

    StreamResult Stream::SetOffset(const size offset)
    {
        // NOTE: Offset is allowed to be at the very end, so that empty streams can be read from and appended to
        if (offset > GetSize())
            return StreamResult::InvalidOffset;

        return StreamResult::Success;
    }

}