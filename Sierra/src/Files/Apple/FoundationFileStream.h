//
// Created by Nikolay Kanchevski on 23.09.24.
//

#pragma once

#if !SR_PLATFORM_APPLE
    #error "Including the FoundationFileStream.h file is only allowed in Apple builds!"
#endif

#if defined(__OBJC__)
    #include <Foundation/Foundation.h>
#else
    namespace Sierra
    {
        using NSFileHandle = void;
    }
#endif

#include "../FileStream.h"

#include "FoundationFileUtilities.h"

namespace Sierra
{

    class SIERRA_API FoundationFileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FoundationFileStream(const FileStreamCreateInfo& createInfo);

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

        [[nodiscard]] const NSFileHandle* GetNSFileHandle() const noexcept { return fileHandle; }

        /* --- COPY SEMANTICS --- */
        FoundationFileStream(const FoundationFileStream&) = delete;
        FoundationFileStream& operator=(const FoundationFileStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        FoundationFileStream(FoundationFileStream&&) noexcept = default;
        FoundationFileStream& operator=(FoundationFileStream&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~FoundationFileStream() noexcept override;

    private:
        NSFileHandle* fileHandle = nil;
        StreamAccess access = StreamAccess::ReadWrite;
        std::filesystem::path filePath = { };

    };

}