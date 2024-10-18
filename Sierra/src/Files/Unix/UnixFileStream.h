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
        explicit UnixFileStream(int fileDescriptor, const std::filesystem::path& filePath);

        /* --- POLLING METHODS --- */
        void Seek(size offset) override;
        [[nodiscard]] std::vector<uint8> Read(size memorySize) override;
        void Write(const void* memory, size offset, size memorySize) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetCurrentOffset() const override;
        [[nodiscard]] size GetMemorySize() const override;
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
        const std::filesystem::path filePath;
        const int fileDescriptor;

    };
    
}