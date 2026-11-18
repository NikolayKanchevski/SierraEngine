//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "Path.h"

#include "FileStream.h"
#include "../Utilities/TimePoint.h"

namespace Sierra
{

    enum class PathConflictPolicy : uint8
    {
        Overwrite,
        KeepExisting,
        KeepBoth
    };

    struct PathMetadata
    {
        uint32 itemCount = 0;
        size memorySize = 0;

        TimePoint timeCreated = { };
        TimePoint timeLastModified = { };
    };

    class SIERRA_API FileManager
    {
    public:
        /* --- TYPE DEFINITIONS --- */
        using PathEnumerationPredicate = std::function<bool(const Path&)>;

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual bool PathExists(const Path& path) const noexcept = 0;
        [[nodiscard]] virtual PathResult CreateFile(const Path& filePath, PathConflictPolicy conflictPolicy) const;
        [[nodiscard]] virtual PathResult CreateDirectory(const Path& directoryPath, PathConflictPolicy conflictPolicy) const;

        [[nodiscard]] virtual PathResult CopyPath(const Path& sourcePath, const Path& destinationPath, PathConflictPolicy conflictPolicy) const;
        [[nodiscard]] virtual PathResult RenamePath(const Path& path, std::string_view name) const;
        [[nodiscard]] virtual PathResult MovePath(const Path& sourcePath, const Path& destinationPath, PathConflictPolicy conflictPolicy) const;
        [[nodiscard]] virtual PathResult DeletePath(const Path& path) const;

        [[nodiscard]] virtual PathResult OpenFileStream(const FileStreamCreateInfo& createInfo, std::unique_ptr<FileStream>& outFileStream) const = 0;
        virtual PathResult EnumeratePaths(const Path& path, const PathEnumerationPredicate& Predicate) const;

        [[nodiscard]] std::optional<std::variant<PathResult, StreamResult>> WriteFile(const Path& path, std::span<const uint8> memory, PathConflictPolicy conflictPolicy) const;
        std::optional<std::variant<PathResult, StreamResult>> ReadFile(const Path& filePath, std::vector<uint8>& outMemory) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual bool IsFile(const Path& path) const = 0;
        [[nodiscard]] virtual bool IsDirectory(const Path& path) const = 0;
        [[nodiscard]] virtual PathResult GetPathMetadata(const Path& path, PathMetadata& outMetadata) const = 0;

        [[nodiscard]] virtual Path GetApplicationDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetExecutableDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetResourcesDirectoryPath() const noexcept = 0;

        [[nodiscard]] virtual Path GetHomeDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetUserDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetTemporaryDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetCachesDirectoryPath() const noexcept = 0;

        [[nodiscard]] virtual Path GetDesktopDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetDownloadsDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetDocumentsDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetPicturesDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetMusicDirectoryPath() const noexcept = 0;
        [[nodiscard]] virtual Path GetVideosDirectoryPath() const noexcept = 0;

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
        [[nodiscard]] PathResult ResolvePathConflict(const Path& path, PathConflictPolicy conflictPolicy, Path& outResolvedPath) const;

    };

}