//
// Created by Nikolay Kanchevski on 23.09.24.
//

#include "FoundationFileStream.h"

#include "../FileErrors.h"
#include "NSFileErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    FoundationFileStream::FoundationFileStream(const std::filesystem::path& filePath, NSFileHandle* const fileHandle)
        : filePath(filePath), fileHandle(fileHandle)
    {
        SR_THROW_IF(fileHandle == nil, InvalidValueError("Cannot create Foundation file stream, as specified file handle must not be nil"));
        SR_THROW_IF(fileHandle.fileDescriptor == -1, InvalidValueError("Cannot create Foundation file stream, as specified file handle is invalid"));
    }

    void FoundationFileStream::Seek(const size offset)
    {
        FileStream::Seek(offset);

        NSError* error = nil;
        [fileHandle seekToOffset: offset error: &error];
        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not seek to offset [{0}] of file stream", offset), filePath);
    }

    std::vector<uint8> FoundationFileStream::Read(const size memorySize)
    {
        SR_THROW_IF(GetCurrentOffset() + memorySize > GetMemorySize(), InvalidFileRange("Cannot read invalid range from file stream", GetFilePath(), GetCurrentOffset(), memorySize, GetMemorySize()));

        NSError* error = nil;
        NSData* const data = [fileHandle readDataUpToLength: memorySize error: &error];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not read [{0}] bytes from file stream", memorySize), filePath);
        return { reinterpret_cast<const uint8*>(data.bytes), reinterpret_cast<const uint8*>(data.bytes) + memorySize };
    }

    void FoundationFileStream::Write(const void* memory, const size offset, const size memorySize)
    {
        FileStream::Write(memory, offset, memorySize);
        NSData* const data = [NSData dataWithBytesNoCopy: const_cast<void*>(reinterpret_cast<const void*>(reinterpret_cast<const uint8*>(memory) + offset)) length: memorySize];

        NSError* error = nil;
        [fileHandle writeData: data error: &error];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not write [{0}] bytes to file stream", memorySize), filePath);
    }

    /* --- GETTER METHODS --- */

    size FoundationFileStream::GetMemorySize() const
    {
        size initialOffset = GetCurrentOffset();

        ullong memorySize = 0;
        [fileHandle seekToEndReturningOffset: &memorySize error: nil];

        NSError* error = nil;
        [fileHandle seekToOffset: initialOffset error: &error];

        if (error != nil) HandleNSFileError(error, "Could not retrieve memory size of file stream", filePath);
        return memorySize;
    }

    size FoundationFileStream::GetCurrentOffset() const
    {
        ullong offset = 0;

        NSError* error = nil;
        [fileHandle getOffset: &offset error: nil];

        if (error != nil) HandleNSFileError(error, "Could not retrieve current offset within file stream", filePath);
        return offset;
    }

    /* --- DESTRUCTOR --- */

    FoundationFileStream::~FoundationFileStream() noexcept
    {
        [fileHandle closeFile];
    }

}