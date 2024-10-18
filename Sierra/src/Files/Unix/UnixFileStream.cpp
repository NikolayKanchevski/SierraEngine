//
// Created by Nikolay Kanchevski on 10.17.24.
//

#include "UnixFileStream.h"

#include "../FileErrors.h"
#include "UnixFileErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UnixFileStream::UnixFileStream(const int fileDescriptor, const std::filesystem::path& filePath)
        : fileDescriptor(fileDescriptor), filePath(filePath)
    {
        SR_THROW_IF(fileDescriptor == -1, InvalidValueError("Cannot create Unix file stream from invalid file handle"));
    }

    /* --- POLLING METHODS --- */

    void UnixFileStream::Seek(const size offset)
    {
        FileStream::Seek(offset);

        if (lseek(fileDescriptor, static_cast<__off_t>(offset), SEEK_SET) == -1)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not seek to offset [{0}] of file stream", offset), filePath);
        }
    }

    std::vector<uint8> UnixFileStream::Read(const size memorySize)
    {
        SR_THROW_IF(GetCurrentOffset() + memorySize > GetMemorySize(), InvalidFileRange("Cannot read invalid range from file stream", GetFilePath(), GetCurrentOffset(), memorySize, GetMemorySize()));

        std::vector<uint8> data = std::vector<uint8>(memorySize);
        if (read(fileDescriptor, data.data(), memorySize) == -1)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not read [{0}] bytes from file stream", memorySize), filePath);
        }

        return data;
    }

    void UnixFileStream::Write(const void* memory, const size offset, const size memorySize)
    {
        FileStream::Write(memory, offset, memorySize);
        if (write(fileDescriptor, reinterpret_cast<const uint8*>(memory) + offset, memorySize) == -1)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not write [{0}] bytes to file stream", memorySize), filePath);
        }
    }

    /* --- GETTER METHODS --- */

    size UnixFileStream::GetMemorySize() const
    {
        const size initialOffset = GetCurrentOffset();

        const size memorySize = lseek(fileDescriptor, 0, SEEK_END);
        if (memorySize == -1) HandleUnixFileError(errno, "Could not get memory size of file stream", filePath);

        const size offset = lseek(fileDescriptor, static_cast<__off_t>(initialOffset), SEEK_SET);
        if (offset == -1) HandleUnixFileError(errno, "Could not get memory size of file stream", filePath);

        return memorySize;
    }

    size UnixFileStream::GetCurrentOffset() const
    {
        const size offset = lseek(fileDescriptor, 0, SEEK_CUR);
        if (offset == -1) HandleUnixFileError(errno, "Could not get current offset of stream", filePath);
        return offset;
    }

    /* --- DESTRUCTOR --- */

    UnixFileStream::~UnixFileStream() noexcept
    {
        close(fileDescriptor);
    }

}