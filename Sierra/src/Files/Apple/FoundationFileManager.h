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
        FoundationFileManager(const FoundationFileManager&) = delete;
        FoundationFileManager& operator=(const FoundationFileManager&) = delete;

        /* --- MOVE SEMANTICS --- */
        FoundationFileManager(FoundationFileManager&&) = delete;
        FoundationFileManager& operator=(FoundationFileManager&&) = delete;

        /* --- DESTRUCTOR --- */
        ~FoundationFileManager() noexcept override = default;

    private:
        const NSFileManager* fileManager = nil;

    };

}
