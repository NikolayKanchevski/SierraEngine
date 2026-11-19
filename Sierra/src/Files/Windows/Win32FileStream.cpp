//
// Created by Nikolay Kanchevski on 10.16.2024.
//

#include "Win32FileStream.h"

#if !defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
    #define NOMINMAX
#endif
#include <Windows.h>

namespace Sierra
{

    namespace
    {
        // Largest number of bytes a single ReadFile() or WriteFile() call is given, as they take sizes of 32 bits
        constexpr size MAX_TRANSFER_SIZE = 1 << 30;
    }

    /* --- CONSTRUCTORS --- */

    Win32FileStream::Win32FileStream(void* fileHandle, const FileStreamCreateInfo& createInfo)
        : FileStream(createInfo), fileHandle(fileHandle), filePath(createInfo.filePath), access(createInfo.access)
    {
        SR_THROW_IF(fileHandle == nullptr || fileHandle == INVALID_HANDLE_VALUE, InvalidValueError("Cannot create Win32 file stream, as specified file handle must not be [INVALID_HANDLE_VALUE]"));
    }

    /* --- POLLING METHODS --- */

    StreamResult Win32FileStream::Read(const std::span<uint8> outMemory)
    {
        if (const StreamResult result = Stream::Read(outMemory); result != StreamResult::Success)
            return result;

        // NOTE: A single ReadFile() call is allowed to read less than requested, so it is called until everything has been read
        size readByteCount = 0;
        while (readByteCount < outMemory.size())
        {
            const DWORD requestedByteCount = static_cast<DWORD>(std::min(outMemory.size() - readByteCount, MAX_TRANSFER_SIZE));

            DWORD byteCount = 0;
            if (!ReadFile(fileHandle, outMemory.data() + readByteCount, requestedByteCount, &byteCount, nullptr))
                return StreamResult::Unknown;

            // End of file was reached before the whole range could be read, which means the file got shrunk in the meantime
            if (byteCount == 0)
                return StreamResult::InvalidRange;

            readByteCount += byteCount;
        }

        return StreamResult::Success;
    }

    StreamResult Win32FileStream::Write(const std::span<const uint8> memory)
    {
        if (const StreamResult result = Stream::Write(memory); result != StreamResult::Success)
            return result;

        // NOTE: A single WriteFile() call is allowed to write less than requested, so it is called until everything has been written
        size writtenByteCount = 0;
        while (writtenByteCount < memory.size())
        {
            const DWORD requestedByteCount = static_cast<DWORD>(std::min(memory.size() - writtenByteCount, MAX_TRANSFER_SIZE));

            DWORD byteCount = 0;
            if (!WriteFile(fileHandle, memory.data() + writtenByteCount, requestedByteCount, &byteCount, nullptr) || byteCount == 0)
                return StreamResult::Unknown;

            writtenByteCount += byteCount;
        }

        return StreamResult::Success;
    }

    /* --- SETTER METHODS --- */

    StreamResult Win32FileStream::SetOffset(const size offset)
    {
        if (const StreamResult result = Stream::SetOffset(offset); result != StreamResult::Success)
            return result;

        LARGE_INTEGER distance = { };
        distance.QuadPart = static_cast<LONGLONG>(offset);

        if (!SetFilePointerEx(fileHandle, distance, nullptr, FILE_BEGIN))
            return StreamResult::Unknown;

        return StreamResult::Success;
    }

    /* --- GETTER METHODS --- */

    size Win32FileStream::GetOffset() const
    {
        LARGE_INTEGER offset = { };
        [[unlikely]] if (!SetFilePointerEx(fileHandle, { }, &offset, FILE_CURRENT))
        {
            SR_WARNING("Could not retrieve offset within file stream [{0}]", filePath.GetString());
            return 0;
        }

        return static_cast<size>(offset.QuadPart);
    }

    size Win32FileStream::GetSize() const
    {
        LARGE_INTEGER fileSize = { };
        [[unlikely]] if (!GetFileSizeEx(fileHandle, &fileSize))
        {
            SR_WARNING("Could not retrieve size of file stream [{0}]", filePath.GetString());
            return 0;
        }

        return static_cast<size>(fileSize.QuadPart);
    }

    /* --- DESTRUCTOR --- */

    Win32FileStream::~Win32FileStream() noexcept
    {
        CloseHandle(fileHandle);
    }

}
