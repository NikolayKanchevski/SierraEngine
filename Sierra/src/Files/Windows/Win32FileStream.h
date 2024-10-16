//
// Created by Nikolay Kanchevski on 10.16.2024.
//

#pragma once

#include "../FileStream.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Sierra
{

    class SIERRA_API Win32FileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32FileStream(HANDLE fileHandle, const std::filesystem::path& filePath);

        /* --- POLLING METHODS --- */
        void Seek(size offset) override;
        [[nodiscard]] std::vector<uint8> Read(size memorySize) override;
        void Write(const void* memory, size offset, size memorySize) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetCurrentOffset() const override;
        [[nodiscard]] size GetMemorySize() const override;
        [[nodiscard]] const std::filesystem::path& GetFilePath() const noexcept override { return filePath; }

        [[nodiscard]] HANDLE GetFileHandle() const noexcept { return fileHandle; }

        /* --- COPY SEMANTICS --- */
        Win32FileStream(const Win32FileStream&) = delete;
        Win32FileStream& operator=(const Win32FileStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32FileStream(Win32FileStream&&) = delete;
        Win32FileStream& operator=(Win32FileStream&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32FileStream() noexcept override;

    private:
        const std::filesystem::path filePath;
        HANDLE fileHandle = INVALID_HANDLE_VALUE;

    };

}