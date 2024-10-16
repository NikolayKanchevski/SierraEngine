//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#pragma once

#include "../FileManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#undef CreateFile
#undef CopyFile
#undef MoveFile
#undef DeleteFile
#undef CreateDirectory

namespace Sierra
{

    class SIERRA_API Win32FileManager final : public FileManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        Win32FileManager() = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool FileExists(const std::filesystem::path& filePath) const noexcept override;
        [[nodiscard]] std::unique_ptr<FileStream> OpenFileStream(const std::filesystem::path& filePath, FileStreamAccess access, FileStreamBuffering buffering) const override;

        void CreateFile(const std::filesystem::path& filePath, FilePathConflictPolicy conflictPolicy) const override;
        void RenameFile(const std::filesystem::path& filePath, std::string_view name) const override;
        void CopyFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        void MoveFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        void DeleteFile(const std::filesystem::path& filePath) const override;

        [[nodiscard]] bool DirectoryExists(const std::filesystem::path& directoryPath) const noexcept override;
        void EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, bool recursive, const FileEnumerationPredicate& Predicate) const override;

        void CreateDirectory(const std::filesystem::path& directoryPath) const override;
        void RenameDirectory(const std::filesystem::path& directoryPath, std::string_view name) const override;
        void CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        void MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        void DeleteDirectory(const std::filesystem::path& directoryPath) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] FileMetadata GetFileMetadata(const std::filesystem::path& filePath) const override;
        [[nodiscard]] DirectoryMetadata GetDirectoryMetadata(const std::filesystem::path& directoryPath) const override;

        [[nodiscard]] std::filesystem::path GetApplicationDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetExecutableDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetResourcesDirectoryPath() const noexcept override;

        [[nodiscard]] std::filesystem::path GetHomeDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetUserDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetTemporaryDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetCachesDirectoryPath() const noexcept override;

        [[nodiscard]] std::filesystem::path GetDesktopDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetDownloadsDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetDocumentsDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetPicturesDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetMusicDirectoryPath() const noexcept override;
        [[nodiscard]] std::filesystem::path GetVideosDirectoryPath() const noexcept override;

        /* --- COPY SEMANTICS --- */
        Win32FileManager(const Win32FileManager&) = delete;
        Win32FileManager& operator=(const Win32FileManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        Win32FileManager(Win32FileManager&&) = delete;
        Win32FileManager& operator=(Win32FileManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~Win32FileManager() noexcept override = default;

    };

}