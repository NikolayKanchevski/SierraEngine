//
// Created by Nikolay Kanchevski on 10.17.24.
//

#pragma once

#include "../FileStream.h"

namespace Sierra
{

    class SIERRA_API UnixFileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnixFileStream(const FileStreamCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::vector<uint8> Read(size memorySize) override;
        void Write(const void* memory, size memorySize) override;

        /* --- SETTER METHODS --- */
        void SetOffset(size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetOffset() const override;
        [[nodiscard]] size GetSize() const override;

        [[nodiscard]] StreamAccess GetAccess() const noexcept override { return access; }
        [[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept override { return filePath; }

        [[nodiscard]] int GetFileDescriptor() const noexcept { return fileDescriptor; }

        /* --- COPY SEMANTICS --- */
        UnixFileStream(const UnixFileStream&) = delete;
        UnixFileStream& operator=(const UnixFileStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        UnixFileStream(UnixFileStream&&) = delete;
        UnixFileStream& operator=(UnixFileStream&&) = delete;

        /* --- DESTRUCTOR --- */
        ~UnixFileStream() noexcept override;

    private:
        const int fileDescriptor;
        const std::filesystem::path filePath;
        const StreamAccess access = StreamAccess::ReadWrite;
    };
    
}