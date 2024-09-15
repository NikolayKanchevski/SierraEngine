//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#pragma once

#include "../../FileManager.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#undef CreateFile
#undef CopyFile
#undef MoveFile
#undef DeleteFile
#undef CreateDirectory

namespace Sierra
{

    class SIERRA_API Win32FileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Win32FileStream(HANDLE fileHandle);

        /* --- POLLING METHODS --- */
        FileOperationResult Seek(size offset) override;
        FileOperationResult Read(size memorySize, std::vector<uint8>& outData) override;
        FileOperationResult Write(const void* memory, size memorySize, size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetMemorySize() const override;
        [[nodiscard]] size GetCurrentOffset() const override;

        /* --- DESTRUCTOR --- */
        ~Win32FileStream() override;

    private:
        HANDLE fileHandle;

    };

    class SIERRA_API Win32FileManager final : public FileManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        Win32FileManager() = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool FileExists(const std::filesystem::path& filePath) const override;
        FileOperationResult OpenFileStream(const std::filesystem::path& filePath, FileStreamAccess access, FileStreamBuffering buffering, std::unique_ptr<FileStream> &outFileStream) const override;

        FileOperationResult CreateFile(const std::filesystem::path& filePath) const override;
        FileOperationResult RenameFile(const std::filesystem::path& filePath, std::string_view name) const override;
        FileOperationResult CopyFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        FileOperationResult MoveFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        FileOperationResult DeleteFile(const std::filesystem::path& filePath) const override;

        [[nodiscard]] bool DirectoryExists(const std::filesystem::path& directoryPath) const override;
        FileOperationResult EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, std::vector<std::filesystem::path>& outFiles, bool recursive) const override;

        FileOperationResult CreateDirectory(const std::filesystem::path& directoryPath) const override;
        FileOperationResult RenameDirectory(const std::filesystem::path& directoryPath, std::string_view name) const override;
        FileOperationResult CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        FileOperationResult MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        FileOperationResult DeleteDirectory(const std::filesystem::path& directoryPath) const override;

        /* --- GETTER METHODS --- */
        FileOperationResult GetFileMetadata(const std::filesystem::path& filePath, FileMetadata& outMetadata) const override;
        FileOperationResult GetDirectoryMetadata(const std::filesystem::path& directoryPath, DirectoryMetadata& outMetadata) const override;

        [[nodiscard]] std::filesystem::path GetApplicationDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetExecutableDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetResourcesDirectoryPath() const override;

        [[nodiscard]] std::filesystem::path GetHomeDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetUserDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetTemporaryDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetCachesDirectoryPath() const override;

        [[nodiscard]] std::filesystem::path GetDesktopDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetDownloadsDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetDocumentsDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetPicturesDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetMusicDirectoryPath() const override;
        [[nodiscard]] std::filesystem::path GetVideosDirectoryPath() const override;

        /* --- DESTRUCTOR --- */
        ~Win32FileManager() override = default;

    };

}