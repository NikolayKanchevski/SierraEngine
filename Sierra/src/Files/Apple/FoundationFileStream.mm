//
// Created by Nikolay Kanchevski on 23.09.24.
//

#include <utility>

#include "FoundationFileStream.h"
#include "FoundationFileUtilities.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    FoundationFileStream::FoundationFileStream(const FileStreamCreateInfo& createInfo)
        : FileStream(createInfo), access(createInfo.access), filePath(createInfo.filePath)
    {
        NSURL* const URL = PathToNSURL(createInfo.filePath);

        NSError* error = nil;
        switch (createInfo.access)
        {
            case StreamAccess::ReadOnly:      { fileHandle = [NSFileHandle fileHandleForReadingFromURL: URL error: &error]; break; }
            case StreamAccess::WriteOnly:     { fileHandle = [NSFileHandle fileHandleForWritingToURL: URL error: &error];   break; }
            case StreamAccess::ReadWrite:     { fileHandle = [NSFileHandle fileHandleForUpdatingURL: URL error: &error];    break; }
        }
        [URL release];

        // NOTE: From the tests I performed, no POSIX/fcntl/C-API buffering configuration results in any performance difference on Apple platforms, which is why no accounting for buffering is done here

        if (error != nil) HandleNSFileError(error, "Could not open file stream", createInfo.filePath);
    }

    /* --- POLLING METHODS --- */

    std::vector<uint8> FoundationFileStream::Read(const size memorySize)
    {
        SR_THROW_IF(GetOffset() + memorySize > GetSize(), InvalidFileRange("Cannot read invalid range from file stream", GetFilePath(), GetOffset(), memorySize, GetSize()));

        NSError* error = nil;
        NSData* const data = [fileHandle readDataUpToLength: memorySize error: &error];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not read [{0}] bytes from file stream", memorySize), filePath);
        return { reinterpret_cast<const uint8*>(data.bytes), reinterpret_cast<const uint8*>(data.bytes) + memorySize };
    }

    void FoundationFileStream::Write(const void* memory, const size memorySize)
    {
        Stream::Write(memory, memorySize);
        NSData* const data = [NSData dataWithBytesNoCopy: const_cast<void*>(memory) length: memorySize freeWhenDone: NO];

        NSError* error = nil;
        [fileHandle writeData: data error: &error];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not write [{0}] bytes to file stream", memorySize), filePath);
    }

    /* --- SETTER METHODS --- */

    void FoundationFileStream::SetOffset(const size offset)
    {
        Stream::SetOffset(offset);

        NSError* error = nil;
        [fileHandle seekToOffset: offset error: &error];
        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not seek to offset [{0}] of file stream", offset), filePath);
    }

    /* --- GETTER METHODS --- */

    size FoundationFileStream::GetSize() const
    {
        size initialOffset = GetOffset();

        ullong memorySize = 0;
        NSError* error = nil;
        [fileHandle seekToEndReturningOffset: &memorySize error: &error];
        if (error != nil) HandleNSFileError(error, "Could not retrieve size of file stream", filePath);

        [fileHandle seekToOffset: initialOffset error: &error];
        if (error != nil) HandleNSFileError(error, "Could not retrieve size of file stream", filePath);

        return memorySize;
    }

    size FoundationFileStream::GetOffset() const
    {
        ullong offset = 0;

        NSError* error = nil;
        [fileHandle getOffset: &offset error: nil];

        if (error != nil) HandleNSFileError(error, "Could not retrieve offset within file stream", filePath);
        return offset;
    }

    /* --- DESTRUCTOR --- */

    FoundationFileStream::~FoundationFileStream() noexcept
    {
        [fileHandle closeFile];
    }

}