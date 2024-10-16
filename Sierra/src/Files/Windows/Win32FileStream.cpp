//
// Created by Nikolay Kanchevski on 10.16.2024.
//

#include "Win32FileStream.h"

#include "../FileErrors.h"
#include "Win32FileErrorHandler.h"

namespace Sierra
{
    /* --- CONSTRUCTORS --- */

    Win32FileStream::Win32FileStream(HANDLE fileHandle, const std::filesystem::path& filePath)
        : fileHandle(fileHandle), filePath(filePath)
    {
        SR_THROW_IF(fileHandle == INVALID_HANDLE_VALUE, InvalidValueError("Cannot create Win32 file stream, as specified file handle must not be [INVALID_HANDLE_VALUE]"));
    }

    /* --- POLLING METHODS --- */

    void Win32FileStream::Seek(const size offset)
    {
        FileStream::Seek(offset);
        const size currentOffset = GetCurrentOffset();

        const DWORD errorCode = SetFilePointerEx(fileHandle, { static_cast<DWORD>(offset - currentOffset) }, nullptr, FILE_CURRENT);
        if (errorCode == 0) HandleWin32FileError(GetLastError(), SR_FORMAT("Could not seek to offset [{0}] of file", offset), filePath);
    }

    std::vector<uint8> Win32FileStream::Read(const size memorySize)
    {
        SR_THROW_IF(GetCurrentOffset() + memorySize > GetMemorySize(), InvalidFileRange("Cannot read invalid range from file", GetFilePath(), GetCurrentOffset(), memorySize, GetMemorySize()));

        std::vector<uint8> data(memorySize);
        if (!ReadFile(fileHandle, data.data(), memorySize, nullptr, nullptr))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not read [{0}] bytes from file", memorySize), filePath);
        }

        return data;
    }

    void Win32FileStream::Write(const void* memory, const size offset, const size memorySize)
    {
        FileStream::Write(memory, offset, memorySize);
        if (!WriteFile(fileHandle, reinterpret_cast<const uint8*>(memory) + offset, memorySize, nullptr, nullptr))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not write [{0}] bytes to file", memorySize), filePath);
        }
    }

    /* --- GETTER METHODS --- */

    size Win32FileStream::GetMemorySize() const
    {
        LARGE_INTEGER memorySize = { };
        GetFileSizeEx(fileHandle, &memorySize);
        return memorySize.QuadPart;
    }

    size Win32FileStream::GetCurrentOffset() const
    {
        LARGE_INTEGER pointer = { };
        SetFilePointerEx(fileHandle, { 0 }, &pointer, FILE_CURRENT);
        return pointer.QuadPart;
    }

    /* --- DESTRUCTOR --- */

    Win32FileStream::~Win32FileStream() noexcept
    {
        CloseHandle(fileHandle);
    }
}