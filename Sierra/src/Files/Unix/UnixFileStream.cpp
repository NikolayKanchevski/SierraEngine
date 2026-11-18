//
// Created by Nikolay Kanchevski on 10.17.24.
//

#include "UnixFileStream.h"

#include <unistd.h>
#include <sys/stat.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UnixFileStream::UnixFileStream(const int fileDescriptor, const FileStreamCreateInfo& createInfo)
        : FileStream(createInfo), fileDescriptor(fileDescriptor), filePath(createInfo.filePath), access(createInfo.access)
    {
        SR_THROW_IF(fileDescriptor == -1, InvalidValueError("Cannot create Unix file stream, as specified file descriptor must not be -1"));
    }

    /* --- POLLING METHODS --- */

    StreamResult UnixFileStream::Read(const std::span<uint8> outMemory)
    {
        if (const StreamResult result = Stream::Read(outMemory); result != StreamResult::Success)
            return result;

        // NOTE: A single read() call is allowed to read less than requested, so it is called until everything has been read
        size readByteCount = 0;
        while (readByteCount < outMemory.size())
        {
            const ssize_t result = read(fileDescriptor, outMemory.data() + readByteCount, outMemory.size() - readByteCount);
            if (result == -1)
            {
                if (errno == EINTR) continue;
                return StreamResult::Unknown;
            }

            // End of file was reached before the whole range could be read, which means the file got shrunk in the meantime
            if (result == 0)
                return StreamResult::InvalidRange;

            readByteCount += static_cast<size>(result);
        }

        return StreamResult::Success;
    }

    StreamResult UnixFileStream::Write(const std::span<const uint8> memory)
    {
        if (const StreamResult result = Stream::Write(memory); result != StreamResult::Success)
            return result;

        // NOTE: A single write() call is allowed to write less than requested, so it is called until everything has been written
        size writtenByteCount = 0;
        while (writtenByteCount < memory.size())
        {
            const ssize_t result = write(fileDescriptor, memory.data() + writtenByteCount, memory.size() - writtenByteCount);
            if (result == -1)
            {
                if (errno == EINTR) continue;
                return StreamResult::Unknown;
            }

            writtenByteCount += static_cast<size>(result);
        }

        return StreamResult::Success;
    }

    /* --- SETTER METHODS --- */

    StreamResult UnixFileStream::SetOffset(const size offset)
    {
        if (const StreamResult result = Stream::SetOffset(offset); result != StreamResult::Success)
            return result;

        if (lseek(fileDescriptor, static_cast<off_t>(offset), SEEK_SET) == -1)
            return StreamResult::Unknown;

        return StreamResult::Success;
    }

    /* --- GETTER METHODS --- */

    size UnixFileStream::GetOffset() const
    {
        const off_t offset = lseek(fileDescriptor, 0, SEEK_CUR);
        [[unlikely]] if (offset == -1)
        {
            SR_WARNING("Could not retrieve offset within file stream [{0}]", filePath.GetString());
            return 0;
        }

        return static_cast<size>(offset);
    }

    size UnixFileStream::GetSize() const
    {
        struct stat status = { };
        [[unlikely]] if (fstat(fileDescriptor, &status) == -1)
        {
            SR_WARNING("Could not retrieve size of file stream [{0}]", filePath.GetString());
            return 0;
        }

        return static_cast<size>(status.st_size);
    }

    /* --- DESTRUCTOR --- */

    UnixFileStream::~UnixFileStream() noexcept
    {
        close(fileDescriptor);
    }

}
