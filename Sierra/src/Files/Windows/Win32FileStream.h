//
// Created by Nikolay Kanchevski on 10.16.2024.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32FileStream.h file is only allowed in Windows builds!"
#endif

#include "../FileStream.h"

namespace Sierra
{

    class SIERRA_API Win32FileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32FileStream(void* fileHandle, const FileStreamCreateInfo& createInfo);

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

        // NOTE: Handle is a HANDLE, which is spelled out as void*, so that Windows.h does not have to be included here
        [[nodiscard]] void* GetFileHandle() const noexcept { return fileHandle; }

        /* --- COPY SEMANTICS --- */
        Win32FileStream(const Win32FileStream&) = delete;
        Win32FileStream& operator=(const Win32FileStream&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32FileStream(Win32FileStream&&) = delete;
        Win32FileStream& operator=(Win32FileStream&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32FileStream() noexcept override;

    private:
        void* fileHandle = nullptr;
        Path filePath = { };
        StreamAccess access = StreamAccess::ReadWrite;

    };

}
