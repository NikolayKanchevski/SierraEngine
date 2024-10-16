//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "FileErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    FileError::FileError(const std::string_view message, const std::string_view error) noexcept
        : std::runtime_error(SR_FORMAT("{0}! Error: {1}.", message, error))
    {

    }

    /* --- CONSTRUCTORS --- */

    UnknownFileError::UnknownFileError(const std::string_view message, const std::filesystem::path& filePath) noexcept
        : FileError(message, SR_FORMAT("File [{0}] caused an unknown error", filePath.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    FileAccessDeniedError::FileAccessDeniedError(const std::string_view message, const std::filesystem::path& filePath) noexcept
        : FileError(message, SR_FORMAT("File [{0}] requires missing permissions", filePath.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    FileLockedError::FileLockedError(const std::string_view message, const std::filesystem::path& filePath) noexcept
        : FileError(message, SR_FORMAT("File [{0}] is locked", filePath.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    FileCorruptedError::FileCorruptedError(const std::string_view message, const std::filesystem::path& filePath) noexcept
        : FileError(message, SR_FORMAT("File [{0}] is corrupted", filePath.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    FileTooLargeError::FileTooLargeError(const std::string_view message, const std::filesystem::path& filePath) noexcept
        : FileError(message, SR_FORMAT("File [{0}] is too large", filePath.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    FileReadOnlyError::FileReadOnlyError(const std::string_view message, const std::filesystem::path& filePath) noexcept
        : FileError(message, SR_FORMAT("File [{0}] is read-only", filePath.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    InvalidFileOffset::InvalidFileOffset(const std::string_view message, const std::filesystem::path& filePath, const size offset, const size fileSize) noexcept
        : FileError(message, SR_FORMAT("Invalid file offset [{0}] is outside file range [0-{1}] of file [{2}]", offset, fileSize, filePath.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    InvalidFileRange::InvalidFileRange(const std::string_view message, const std::filesystem::path& filePath, const size offset, const size memorySize, const size fileSize) noexcept
        : FileError(message, SR_FORMAT("Invalid file range [{0}-{1}] is outside file range [0-{2}] of file [{3}].", offset, offset + memorySize, fileSize, filePath.string()))
    {

    }

}
