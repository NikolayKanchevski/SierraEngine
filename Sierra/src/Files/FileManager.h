//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "FileStream.h"
#include "../Utilities/Date.h"

namespace Sierra
{

    enum class FilePathConflictPolicy : uint8
    {
        Overwrite,
        KeepExisting,
        KeepBoth
    };

    enum class FileType : uint8
    {
        Unknown,
        Text,
        Document,
        Image,
        Audio,
        Video,
        Archive,
        Executable,
        Configuration
    };

    struct FileMetadata
    {
        FileType type = FileType::Unknown;
        size memorySize = 0;

        Date dateCreated = { };
        Date dateLastModified = { };
    };

    struct DirectoryMetadata
    {
        uint32 fileCount = 0;
        size memorySize = 0;

        Date dateCreated = { };
        Date dateLastModified = { };
    };

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API FileType FilePathToFileType(const std::filesystem::path& filePath) noexcept;

    class SIERRA_API FileManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using FileEnumerationPredicate = std::function<void(const std::filesystem::path&)>;

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual bool FileExists(const std::filesystem::path& filePath) const noexcept = 0;
        [[nodiscard]] virtual std::unique_ptr<FileStream> OpenFileStream(const std::filesystem::path& filePath, FileStreamAccess access, FileStreamBuffering buffering) const = 0;

        virtual void CreateFile(const std::filesystem::path& filePath) const = 0;
        virtual void RenameFile(const std::filesystem::path& filePath, std::string_view name) const = 0;
        virtual void CopyFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual void MoveFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual void DeleteFile(const std::filesystem::path& filePath) const = 0;

        [[nodiscard]] virtual bool DirectoryExists(const std::filesystem::path& directoryPath) const noexcept = 0;
        virtual void EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, const bool recursive, const FileEnumerationPredicate& Predicate) const = 0;

        virtual void CreateDirectory(const std::filesystem::path& directoryPath) const = 0;
        virtual void RenameDirectory(const std::filesystem::path& directoryPath, std::string_view name) const = 0;
        virtual void CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual void MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual void DeleteDirectory(const std::filesystem::path& directoryPath) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual FileMetadata GetFileMetadata(const std::filesystem::path& filePath) const = 0;
        [[nodiscard]] virtual DirectoryMetadata GetDirectoryMetadata(const std::filesystem::path& directoryPath) const = 0;

        [[nodiscard]] virtual std::filesystem::path GetApplicationDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetExecutableDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetResourcesDirectoryPath() const noexcept = 0;

        [[nodiscard]] virtual std::filesystem::path GetHomeDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetUserDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetTemporaryDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetCachesDirectoryPath() const noexcept = 0;

        [[nodiscard]] virtual std::filesystem::path GetDesktopDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetDownloadsDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetDocumentsDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetPicturesDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetMusicDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual std::filesystem::path GetVideosDirectoryPath() const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        FileManager(const FileManager&) = delete;
        FileManager& operator=(const FileManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        FileManager(FileManager&&) = delete;
        FileManager& operator=(FileManager&&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~FileManager() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        FileManager() noexcept = default;

        /* --- POLLING METHODS --- */
        void ResolveFilePathConflict(const std::filesystem::path& sourceFilePath, std::filesystem::path& destinationFilePath, FilePathConflictPolicy conflictPolicy) const;

    };

}