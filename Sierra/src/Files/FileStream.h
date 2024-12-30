//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

#include "../Utilities/Stream.h"

#include "FileErrors.h"
#include "PathErrors.h"

namespace Sierra
{

    enum class FileStreamBuffering : uint8
    {
        Buffered,
        Unbuffered
    };

    struct FileStreamCreateInfo
    {
        std::filesystem::path filePath = { };
        StreamAccess access = StreamAccess::ReadWrite;
        FileStreamBuffering buffering = FileStreamBuffering::Unbuffered;
    };

    class SIERRA_API FileStream : public Stream
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const std::filesystem::path& GetFilePath() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        FileStream(const FileStream&) = delete;
        FileStream& operator=(const FileStream&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileStream() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit FileStream(const FileStreamCreateInfo& createInfo) noexcept;

        /* --- MOVE SEMANTICS --- */
        FileStream(FileStream&&) noexcept = default;
        FileStream& operator=(FileStream&&) noexcept = default;

    };

}
