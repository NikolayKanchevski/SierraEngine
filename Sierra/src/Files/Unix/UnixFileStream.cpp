//
// Created by Nikolay Kanchevski on 10.17.24.
//

#include "UnixFileStream.h"

#include <fcntl.h>
#include <unistd.h>

#include "UnixFileErrorHandler.h"

namespace Sierra
{

    namespace
    {
        [[nodiscard]] int StreamAccessToFlags(const StreamAccess access) noexcept
        {
            int flags = 0;
            switch (access)
            {
                case StreamAccess::ReadOnly:  { flags |= O_RDONLY; break; }
                case StreamAccess::WriteOnly: { flags |= O_WRONLY; break; }
                case StreamAccess::ReadWrite: { flags |= O_RDWR;   break; }
                default:                          break;
            }

            return flags;
        }

        [[nodiscard]] int FileStreamBufferingToFlags(const FileStreamBuffering buffering)
        {
            return static_cast<uint8>(buffering == FileStreamBuffering::Unbuffered) * (O_SYNC | O_DIRECT);
        }
    }

    /* --- CONSTRUCTORS --- */

    UnixFileStream::UnixFileStream(const FileStreamCreateInfo& createInfo)
        : FileStream(createInfo),
          fileDescriptor(open(createInfo.filePath.c_str(), StreamAccessToFlags(createInfo.access) | FileStreamBufferingToFlags(createInfo.buffering))),
          filePath(createInfo.filePath),
          access(createInfo.access)
    {
        if (fileDescriptor == -1) HandleUnixFileError(errno, "Could not open file stream", filePath);
    }

    /* --- POLLING METHODS --- */

    std::vector<uint8> UnixFileStream::Read(const size memorySize)
    {
        SR_THROW_IF(GetOffset() + memorySize > GetSize(), InvalidFileRange("Cannot read invalid range from file stream", GetFilePath(), GetOffset(), memorySize, GetSize()));

        std::vector<uint8> data = std::vector<uint8>(memorySize);
        if (read(fileDescriptor, data.data(), memorySize) == -1)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not read [{0}] bytes from file stream", memorySize), filePath);
        }

        return data;
    }

    void UnixFileStream::Write(const void* memory, const size memorySize)
    {
        Stream::Write(memory, memorySize);
        if (write(fileDescriptor, memory, memorySize) == -1)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not write [{0}] bytes to file stream", memorySize), filePath);
        }
    }

    /* --- SETTER METHODS --- */

    void UnixFileStream::SetOffset(const size offset)
    {
        Stream::SetOffset(offset);

        if (lseek(fileDescriptor, static_cast<__off_t>(offset), SEEK_SET) == -1)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not seek to offset [{0}] of file stream", offset), filePath);
        }
    }

    /* --- GETTER METHODS --- */

    size UnixFileStream::GetSize() const
    {
        const size initialOffset = GetOffset();

        const size memorySize = lseek(fileDescriptor, 0, SEEK_END);
        if (memorySize == -1) HandleUnixFileError(errno, "Could not get memory size of file stream", filePath);

        const size offset = lseek(fileDescriptor, static_cast<__off_t>(initialOffset), SEEK_SET);
        if (offset == -1) HandleUnixFileError(errno, "Could not get memory size of file stream", filePath);

        return memorySize;
    }

    size UnixFileStream::GetOffset() const
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