//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "FileStream.h"

#include "FileErrors.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void FileStream::Seek(const size offset)
    {
        SR_THROW_IF(offset >= GetMemorySize(), InvalidFileOffset("Cannot seek to invalid offset in file", GetFilePath(), offset, GetMemorySize()));
    }

    void FileStream::SeekToEnd()
    {
        return Seek(GetMemorySize() - 1);
    }

    std::vector<uint8> FileStream::Read(const size offset, const size memorySize)
    {
        Seek(offset);
        return Read(memorySize);
    }

    void FileStream::Write(const void* memory, const size sourceOffset, const size memorySize)
    {
        SR_THROW_IF(memory == nullptr, InvalidValueError(SR_FORMAT("Cannot write memory range [{0}-{1}] to file [{2}], as specified memory pointer must not be null", sourceOffset, sourceOffset + memorySize, GetFilePath().string())));
    }

    void FileStream::Write(const void* memory, const size sourceOffset, const size destinationOffset, const size memorySize)
    {
        Seek(destinationOffset);
        Write(memory, sourceOffset, memorySize);
    }

}
