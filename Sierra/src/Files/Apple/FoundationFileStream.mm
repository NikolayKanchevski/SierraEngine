//
// Created by Nikolay Kanchevski on 23.09.24.
//

#include <utility>

#include "FoundationFileStream.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    FoundationFileStream::FoundationFileStream(NSFileHandle* fileHandle, const FileStreamCreateInfo& createInfo)
        : FileStream(createInfo), fileHandle(fileHandle), access(createInfo.access), filePath(createInfo.filePath)
    {
        SR_THROW_IF(fileHandle == nil, InvalidValueError("File handle must not be nil"));
    }

    /* --- POLLING METHODS --- */

    StreamResult FoundationFileStream::Read(const std::span<uint8> outMemory)
    {
        if (const StreamResult result = Stream::Read(outMemory); result != StreamResult::Success)
            return result;

        NSError* error = nil;
        NSData* const data = [fileHandle readDataUpToLength: outMemory.size() error: &error];

        if (error != nil)
            return StreamResult::Unknown;

        std::memcpy(outMemory.data(), reinterpret_cast<const uint8*>(data.bytes), outMemory.size());
        return StreamResult::Success;
    }

    StreamResult FoundationFileStream::Write(const std::span<const uint8> memory)
    {
        if (const StreamResult result = Stream::Write(memory); result != StreamResult::Success)
            return result;

        NSData* const data = [NSData dataWithBytesNoCopy: (void*) memory.data() length:memory.size() freeWhenDone:NO];

        NSError* error = nil;
        [fileHandle writeData: data error: &error];

        if (error != nil)
            return StreamResult::Unknown;

        return StreamResult::Success;
    }

    /* --- SETTER METHODS --- */

    StreamResult FoundationFileStream::SetOffset(const size offset)
    {
        if (const StreamResult result = Stream::SetOffset(offset); result != StreamResult::Success)
            return result;

        NSError* error = nil;
        [fileHandle seekToOffset: offset error: &error];

        if (error != nil)
            return StreamResult::Unknown;

        return StreamResult::Success;
    }

    /* --- GETTER METHODS --- */

    size FoundationFileStream::GetOffset() const
    {
        ullong offset = 0;

        NSError* error = nil;
        [fileHandle getOffset: &offset error: nil];

        [[unlikely]] if (error != nil)
        {
            SR_WARNING("Could not retrieve offset within file stream [{0}]", filePath.GetString());
            return 0;
        }

        return offset;
    }

    size FoundationFileStream::GetSize() const
    {
        size initialOffset = GetOffset();

        ullong memorySize = 0;
        NSError* error = nil;
        [fileHandle seekToEndReturningOffset: &memorySize error: &error];

        [[unlikely]] if (error != nil)
        {
            SR_WARNING("Could not seek to end to retrieve size of file stream [{0}]", filePath.GetString());
            return 0;
        }

        [fileHandle seekToOffset: initialOffset error: &error];
        [[unlikely]] if (error != nil)
        {
            SR_WARNING("Could not seek back after retrieving size of file stream [{0}]", filePath.GetString());
            return 0;
        }

        return memorySize;
    }

    /* --- DESTRUCTOR --- */

    FoundationFileStream::~FoundationFileStream() noexcept
    {
        [fileHandle closeFile];
    }

}