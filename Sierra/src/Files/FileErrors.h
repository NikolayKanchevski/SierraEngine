//
// Created by Nikolay Kanchevski on 22.09.24.
//

#pragma once

namespace Sierra
{

    class SIERRA_API FileError : public std::runtime_error
    {
    public:
        /* --- COPY SEMANTICS --- */
        FileError(const FileError&) = delete;
        FileError& operator=(const FileError&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileError(FileError&&) = delete;
        FileError& operator=(FileError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileError() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        FileError(std::string_view message, std::string_view error) noexcept;

    };

    class SIERRA_API UnknownFileError final : public FileError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit UnknownFileError(std::string_view message, const std::filesystem::path& filePath) noexcept;

        /* --- COPY SEMANTICS --- */
        UnknownFileError(const UnknownFileError&) = delete;
        UnknownFileError& operator=(const UnknownFileError&) = delete;

        /* --- MOVE SEMANTICS --- */
        UnknownFileError(UnknownFileError&&) = delete;
        UnknownFileError& operator=(UnknownFileError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~UnknownFileError() noexcept override = default;
    };

    class SIERRA_API FileAccessDeniedError final : public FileError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FileAccessDeniedError(std::string_view message, const std::filesystem::path& filePath) noexcept;

        /* --- COPY SEMANTICS --- */
        FileAccessDeniedError(const FileAccessDeniedError&) = delete;
        FileAccessDeniedError& operator=(const FileAccessDeniedError&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileAccessDeniedError(FileAccessDeniedError&&) = delete;
        FileAccessDeniedError& operator=(FileAccessDeniedError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileAccessDeniedError() noexcept override = default;
    };

    class SIERRA_API FileLockedError final : public FileError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FileLockedError(std::string_view message, const std::filesystem::path& filePath) noexcept;

        /* --- COPY SEMANTICS --- */
        FileLockedError(const FileLockedError&) = delete;
        FileLockedError& operator=(const FileLockedError&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileLockedError(FileLockedError&&) = delete;
        FileLockedError& operator=(FileLockedError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileLockedError() noexcept override = default;
    };

    class SIERRA_API FileCorruptedError final : public FileError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FileCorruptedError(std::string_view message, const std::filesystem::path& filePath) noexcept;

        /* --- COPY SEMANTICS --- */
        FileCorruptedError(const FileCorruptedError&) = delete;
        FileCorruptedError& operator=(const FileCorruptedError&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileCorruptedError(FileCorruptedError&&) = delete;
        FileCorruptedError& operator=(FileCorruptedError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileCorruptedError() noexcept override = default;
    };

    class SIERRA_API FileTooLargeError final : public FileError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FileTooLargeError(std::string_view message, const std::filesystem::path& filePath) noexcept;

        /* --- COPY SEMANTICS --- */
        FileTooLargeError(const FileTooLargeError&) = delete;
        FileTooLargeError& operator=(const FileTooLargeError&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileTooLargeError(FileTooLargeError&&) = delete;
        FileTooLargeError& operator=(FileTooLargeError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileTooLargeError() noexcept override = default;
    };

    class SIERRA_API FileReadOnlyError final : public FileError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FileReadOnlyError(std::string_view message, const std::filesystem::path& filePath) noexcept;

        /* --- COPY SEMANTICS --- */
        FileReadOnlyError(const FileReadOnlyError&) = delete;
        FileReadOnlyError& operator=(const FileReadOnlyError&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileReadOnlyError(FileReadOnlyError&&) = delete;
        FileReadOnlyError& operator=(FileReadOnlyError&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileReadOnlyError() noexcept override = default;
    };

    class SIERRA_API InvalidFileRange final : public FileError
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit InvalidFileRange(const std::string_view message, const std::filesystem::path& filePath, const size offset, const size memorySize, const size fileSize) noexcept;

        /* --- COPY SEMANTICS --- */
        InvalidFileRange(const InvalidFileRange&) = delete;
        InvalidFileRange& operator=(const InvalidFileRange&) = delete;

        /* --- MOVE SEMANTICS --- */
        InvalidFileRange(InvalidFileRange&&) = delete;
        InvalidFileRange& operator=(InvalidFileRange&&) = delete;

        /* --- DESTRUCTOR --- */
        ~InvalidFileRange() noexcept override = default;
    };

}