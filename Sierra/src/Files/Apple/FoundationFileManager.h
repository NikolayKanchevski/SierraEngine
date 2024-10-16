//
// Created by Nikolay Kanchevski on 27.06.24.
//

#pragma once

#if !SR_PLATFORM_APPLE
    #error "Including the FoundationFileManager.h file is only allowed in Apple builds!"
#endif

#if defined(__OBJC__)
    #include <Foundation/Foundation.h>
#else
    namespace Sierra
    {
        #define nil nullptr
        using NSFileManager = void;
        using NSURL = void;
    }
#endif

#include "../FileManager.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */
    [[nodiscard]] SIERRA_API NSURL* PathToNSURL(const std::filesystem::path& path) noexcept;

    struct FoundationFileManagerCreateInfo
    {
        NSFileManager* fileManager = nil;
    };

    class SIERRA_API FoundationFileManager final : public FileManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FoundationFileManager(const FoundationFileManagerCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool FileExists(const std::filesystem::path& filePath) const noexcept override;
        [[nodiscard]] std::unique_ptr<FileStream> OpenFileStream(const std::filesystem::path& filePath, FileStreamAccess access, FileStreamBuffering buffering) const override;

        void CreateFile(const std::filesystem::path& filePath) const override;
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

        /* --- DESTRUCTOR --- */
        ~FoundationFileManager() noexcept override = default;

    private:
        const NSFileManager* const fileManager = nil;

    };

}
