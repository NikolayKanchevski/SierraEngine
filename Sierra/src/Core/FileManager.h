//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "Time.h"

namespace Sierra
{

    enum class FileOperationResult : uint8
    {
        Success,
        UnknownError,
        FilePathInvalid,
        NoSuchFilePath,
        FilePathAlreadyExists,
        FileLocked,
        FileCorrupted,
        MissingPermissions,
        VolumeReadOnly,
        VolumeOutOfSpace
    };

    enum class FileStreamAccess : uint8
    {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

    enum class FileStreamBuffering : uint8
    {
        Buffered,
        Unbuffered
    };

    class SIERRA_API FileStream
    {
    public:
        /* --- POLLING METHODS --- */
        virtual FileOperationResult Seek(uint64 offset) = 0;
        virtual FileOperationResult SeekToEnd() = 0;

        virtual FileOperationResult Read(uint64 memorySize, std::vector<uint8> &outData) = 0;
        virtual FileOperationResult Write(const void* memoryPointer, uint64 memorySize) = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual uint64 GetCurrentOffset() const = 0;

        /* --- OPERATORS --- */
        FileStream(const FileStream&) = delete;
        FileStream& operator=(const FileStream&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~FileStream() = default;

    protected:
        FileStream() = default;

    };

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
        uint64 memorySize = 0;

        Date dateCreated;
        Date dateLastModified;
    };

    struct DirectoryMetadata
    {
        uint32 fileCount = 0;
        uint64 memorySize = 0;

        Date dateCreated;
        Date dateLastModified;
    };

    class SIERRA_API FileManager
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual bool FileExists(const std::filesystem::path &filePath) const = 0;
        virtual FileOperationResult OpenFileStream(const std::filesystem::path &filePath, FileStreamAccess access, FileStreamBuffering buffering, std::unique_ptr<FileStream> &outFileStream) const = 0;

        virtual FileOperationResult CreateFile(const std::filesystem::path &filePath) const = 0;
        virtual FileOperationResult RenameFile(const std::filesystem::path &filePath, std::string_view name) const = 0;
        virtual FileOperationResult CopyFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path &destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual FileOperationResult MoveFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path &destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual FileOperationResult DeleteFile(const std::filesystem::path &filePath) const = 0;

        [[nodiscard]] virtual bool DirectoryExists(const std::filesystem::path &directoryPath) const = 0;
        virtual FileOperationResult EnumerateDirectoryFiles(const std::filesystem::path &directoryPath, std::vector<std::filesystem::path> &outFiles, bool recursive) const = 0;

        virtual FileOperationResult CreateDirectory(const std::filesystem::path &directoryPath) const = 0;
        virtual FileOperationResult RenameDirectory(const std::filesystem::path &directoryPath, std::string_view name) const = 0;
        virtual FileOperationResult CopyDirectory(const std::filesystem::path &sourceDirectoryPath, const std::filesystem::path &destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual FileOperationResult MoveDirectory(const std::filesystem::path &sourceDirectoryPath, const std::filesystem::path &destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const = 0;
        virtual FileOperationResult DeleteDirectory(const std::filesystem::path &directoryPath) const = 0;

        /* --- GETTER METHODS --- */
        virtual FileOperationResult GetFileMetadata(const std::filesystem::path &filePath, FileMetadata &outMetadata) const = 0;
        virtual FileOperationResult GetDirectoryMetadata(const std::filesystem::path &directoryPath, DirectoryMetadata &outMetadata) const = 0;

        [[nodiscard]] virtual std::filesystem::path GetApplicationDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetExecutableDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetResourcesDirectoryPath() const = 0;

        [[nodiscard]] virtual std::filesystem::path GetHomeDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetUserDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetTemporaryDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetCachesDirectoryPath() const = 0;

        [[nodiscard]] virtual std::filesystem::path GetDesktopDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetDownloadsDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetDocumentsDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetPicturesDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetMusicDirectoryPath() const = 0;
        [[nodiscard]] virtual std::filesystem::path GetVideosDirectoryPath() const = 0;

        /* --- CONVERSIONS --- */
        [[nodiscard]] static FileType FilePathToFileType(const std::filesystem::path &filePath);

        /* --- OPERATORS --- */
        FileManager(const FileManager&) = delete;
        FileManager& operator=(const FileManager&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~FileManager() = default;

    protected:
        FileManager() = default;

    };

}
