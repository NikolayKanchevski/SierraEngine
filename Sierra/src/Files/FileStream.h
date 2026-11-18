//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

#include "Path.h"
#include "../Utilities/Stream.h"

namespace Sierra
{

    enum class FileStreamBuffering : bool
    {
        Unbuffered,
        Buffered
    };

    struct FileStreamCreateInfo
    {
        const Path& filePath = { };
        StreamAccess access = StreamAccess::ReadWrite;
        FileStreamBuffering buffering = FileStreamBuffering::Unbuffered;
    };

    class SIERRA_API FileStream : public virtual Stream
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual const Path& GetFilePath() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        FileStream(const FileStream&) = delete;
        FileStream& operator=(const FileStream&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~FileStream() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        explicit FileStream(const FileStreamCreateInfo& createInfo) noexcept;

        /* --- MOVE SEMANTICS --- */
        FileStream(FileStream&&) noexcept = default;
        FileStream& operator=(FileStream&&) noexcept = default;

    };

}
