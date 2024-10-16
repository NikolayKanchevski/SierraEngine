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

#include "NSFileErrorHandler.h"

namespace Sierra
{

    class SIERRA_API FoundationFileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        FoundationFileStream(const std::filesystem::path& filePath, NSFileHandle* fileHandle);

        /* --- POLLING METHODS --- */
        void Seek(size offset) override;
        [[nodiscard]] std::vector<uint8> Read(size memorySize) override;
        void Write(const void* memory, size sourceOffset, size memorySize) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetCurrentOffset() const override;
        [[nodiscard]] size GetMemorySize() const override;
        [[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept override { return filePath; }

        [[nodiscard]] const NSFileHandle* GetNSFileHandle() const noexcept { return fileHandle; }

        /* --- COPY SEMANTICS --- */
        FoundationFileStream(const FoundationFileStream&) = delete;
        FoundationFileStream& operator=(const FoundationFileStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        FoundationFileStream(FoundationFileStream&&) = delete;
        FoundationFileStream& operator=(FoundationFileStream&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FoundationFileStream() noexcept override;

    private:
        const std::filesystem::path filePath;
        const NSFileHandle* fileHandle = nil;

    };

}