//
// Created by Nikolay Kanchevski on 27.06.24.
//

#pragma once

#include "../../FileManager.h"

#if defined(__OBJC__)
    #include <Foundation/Foundation.h>
#else
    namespace Sierra
    {
        using NSFileHandle = void;
        using NSFileManager = void;
        using NSURL = void;
        using NSError = void;
    }
#endif

namespace Sierra
{

    class SIERRA_API FoundationFileStream final : public FileStream
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FoundationFileStream(NSFileHandle* fileHandle);

        /* --- POLLING METHODS --- */
        FileOperationResult Seek(size offset) override;
        FileOperationResult Read(size memorySize, std::vector<uint8>& outData) override;
        FileOperationResult Write(const void* memory, size memorySize, size offset) override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetMemorySize() const override;
        [[nodiscard]] size GetCurrentOffset() const override;

        [[nodiscard]] NSFileHandle* GetNSFileHandle() const { return fileHandle; }

        /* --- DESTRUCTOR --- */
        ~FoundationFileStream() override;

    private:
        NSFileHandle* fileHandle;

    };

    class SIERRA_API FoundationFileManager final : public FileManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        FoundationFileManager();

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool FileExists(const std::filesystem::path& filePath) const override;
        FileOperationResult OpenFileStream(const std::filesystem::path& filePath, FileStreamAccess access, FileStreamBuffering buffering, std::unique_ptr<FileStream>& outFileStream) const override;

        FileOperationResult CreateFile(const std::filesystem::path& filePath) const override;
        FileOperationResult RenameFile(const std::filesystem::path& filePath, std::string_view name) const override;
        FileOperationResult CopyFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
        FileOperationResult MoveFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const override;
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

        /* --- CONVERSIONS --- */
        [[nodiscard]] static NSURL* PathToNSURL(const std::filesystem::path& path);

        /* --- DESTRUCTOR --- */
        ~FoundationFileManager() override = default;

    private:
        NSFileManager* const fileManager = nullptr;

    };

}
