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

namespace Sierra
{

    class SIERRA_API FoundationFileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FoundationFileStream(NSFileHandle* fileHandle, const FileStreamCreateInfo& createInfo);

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

        Path filePath = { };
        StreamAccess access = StreamAccess::ReadWrite;

    };

}