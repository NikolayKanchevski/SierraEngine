//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "FileStream.h"

#include "FileErrors.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    void FileStream::SeekToEnd()
    {
        return Seek(GetMemorySize() - 1);
    }

    std::vector<uint8> FileStream::Read(const size offset, const size memorySize)
    {
        Seek(offset);
        return Read(memorySize);
    }

    void FileStream::Write(const void* memory, const size sourceOffset, const size destinationOffset, const size memorySize)
    {
        Seek(destinationOffset);
        Write(memory, memorySize, sourceOffset);
    }

}
