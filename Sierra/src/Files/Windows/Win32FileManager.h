//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the Win32FileManager.h file is only allowed in Windows builds!"
#endif

#include "../FileManager.h"

namespace Sierra
{

    class SIERRA_API Win32FileManager final : public FileManager
    {
    public:
        /* --- CONSTRUCTORS --- */
        Win32FileManager() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool PathExists(const Path& path) const noexcept override;
        [[nodiscard]] PathResult CreateFile(const Path& filePath, PathConflictPolicy conflictPolicy) const override;
        [[nodiscard]] PathResult CreateDirectory(const Path& directoryPath, PathConflictPolicy conflictPolicy) const override;

        [[nodiscard]] PathResult CopyPath(const Path& sourcePath, const Path& destinationPath, PathConflictPolicy conflictPolicy) const override;
        [[nodiscard]] PathResult RenamePath(const Path& path, std::string_view name) const override;
        [[nodiscard]] PathResult MovePath(const Path& sourcePath, const Path& destinationPath, PathConflictPolicy conflictPolicy) const override;
        [[nodiscard]] PathResult DeletePath(const Path& path) const override;

        [[nodiscard]] PathResult OpenFileStream(const FileStreamCreateInfo& createInfo, std::unique_ptr<FileStream>& outFileStream) const override;
        PathResult EnumeratePaths(const Path& path, const PathEnumerationPredicate& Predicate) const override;

        /* --- GETTER METHODS --- */
        [[nodiscard]] bool IsFile(const Path& path) const override;
        [[nodiscard]] bool IsDirectory(const Path& path) const override;
        [[nodiscard]] PathResult GetPathMetadata(const Path& path, PathMetadata& outMetadata) const override;

        [[nodiscard]] Path GetApplicationDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetExecutableDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetResourcesDirectoryPath() const noexcept override;

        [[nodiscard]] Path GetHomeDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetUserDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetTemporaryDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetCachesDirectoryPath() const noexcept override;

        [[nodiscard]] Path GetDesktopDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetDownloadsDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetDocumentsDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetPicturesDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetMusicDirectoryPath() const noexcept override;
        [[nodiscard]] Path GetVideosDirectoryPath() const noexcept override;

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
