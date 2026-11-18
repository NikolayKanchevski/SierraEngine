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
        explicit UnixFileStream(int fileDescriptor, const FileStreamCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] StreamResult Read(std::span<uint8> outMemory) override;
        [[nodiscard]] StreamResult Write(std::span<const uint8> memory) override;

        /* --- SETTER METHODS --- */
        [[nodiscard]] StreamResult SetOffset(size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const Path& GetFilePath() const noexcept override { return filePath; }
        [[nodiscard]] size GetOffset() const override;
        [[nodiscard]] size GetSize() const override;
        [[nodiscard]] StreamAccess GetAccess() const noexcept override { return access; }

        [[nodiscard]] int GetFileDescriptor() const noexcept { return fileDescriptor; }

        /* --- COPY SEMANTICS --- */
        UnixFileStream(const UnixFileStream&) = delete;
        UnixFileStream& operator=(const UnixFileStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        UnixFileStream(UnixFileStream&&) noexcept = default;
        UnixFileStream& operator=(UnixFileStream&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~UnixFileStream() noexcept override;

    private:
        int fileDescriptor = 0;
        Path filePath = { };
        StreamAccess access = StreamAccess::ReadWrite;
    };
    
}