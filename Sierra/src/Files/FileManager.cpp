//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "FileManager.h"

namespace Sierra
{

    namespace
    {
        [[nodiscard]] Path GetCanonicalPath(const Path& path)
        {
            std::error_code errorCode = { };
            std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path.GetStandardPath(), errorCode);
            return errorCode ? path.GetNormalizedPath() : Path(std::move(canonicalPath));
        }

        [[nodiscard]] PathResult ValidatePathTransfer(const FileManager& fileManager, const Path& sourcePath, const Path& destinationPath, const PathConflictPolicy conflictPolicy)
        {
            if (!fileManager.PathExists(sourcePath))
                return PathResult::NotFound;

            const Path canonicalSourcePath = GetCanonicalPath(sourcePath);
            const Path canonicalDestinationItemPath = GetCanonicalPath(destinationPath / sourcePath.GetFileName());

            // NOTE: A directory cannot be transferred into itself
            if (canonicalSourcePath.IsAncestor(canonicalDestinationItemPath))
                return PathResult::Unknown;

            // NOTE: A path can be transferred onto itself only by keeping both, as overwriting it would delete it before the transfer
            if (canonicalSourcePath == canonicalDestinationItemPath && conflictPolicy != PathConflictPolicy::KeepBoth)
                return PathResult::AlreadyExists;

            return PathResult::Success;
        }
    }

    /* --- POLLING METHODS --- */

    PathResult FileManager::CreateFile(const Path& filePath, const PathConflictPolicy conflictPolicy) const
    {
        return PathResult::Success;
    }

    PathResult FileManager::CreateDirectory(const Path& directoryPath, const PathConflictPolicy conflictPolicy) const
    {
        return PathResult::Success;
    }

    PathResult FileManager::CopyPath(const Path& sourcePath, const Path& destinationDirectoryPath, const PathConflictPolicy conflictPolicy) const
    {
        return ValidatePathTransfer(*this, sourcePath, destinationDirectoryPath, conflictPolicy);
    }

    PathResult FileManager::RenamePath(const Path& path, const std::string_view name) const
    {
        if (!PathExists(path))
            return PathResult::NotFound;

        if (PathExists(path.GetParentPath() / name))
            return PathResult::AlreadyExists;

        return PathResult::Success;
    }

    PathResult FileManager::MovePath(const Path& sourcePath, const Path& destinationDirectoryPath, const PathConflictPolicy conflictPolicy) const
    {
        return ValidatePathTransfer(*this, sourcePath, destinationDirectoryPath, conflictPolicy);
    }

    PathResult FileManager::DeletePath(const Path& path) const
    {
        if (!PathExists(path))
            return PathResult::NotFound;

        return PathResult::Success;
    }

    PathResult FileManager::EnumeratePaths(const Path& path, const PathEnumerationPredicate& Predicate) const
    {
        if (!PathExists(path))
            return PathResult::NotFound;

        return PathResult::Success;
    }

    std::optional<std::variant<PathResult, StreamResult>> FileManager::WriteFile(const Path& filePath, const std::span<const uint8> memory, const PathConflictPolicy conflictPolicy) const
    {
        Path resolvedFilePath = filePath;
        if (const PathResult result = ResolvePathConflict(filePath, conflictPolicy, resolvedFilePath); result != PathResult::Success)
            return result;

        // NOTE: Conflict has already been resolved, so nothing exists at the resolved path, and the file gets created empty
        if (const PathResult result = CreateFile(resolvedFilePath, PathConflictPolicy::KeepExisting); result != PathResult::Success)
            return result;

        std::unique_ptr<FileStream> stream = nullptr;
        if (const PathResult result = OpenFileStream({ .filePath = resolvedFilePath, .access = StreamAccess::WriteOnly }, stream); result != PathResult::Success)
            return result;

        if (const StreamResult result = stream->Write(memory); result != StreamResult::Success)
            return result;

        return std::nullopt;
    }

    std::optional<std::variant<PathResult, StreamResult>> FileManager::ReadFile(const Path& filePath, std::vector<uint8>& outMemory) const
    {
        std::unique_ptr<FileStream> stream = nullptr;
        if (const PathResult result = OpenFileStream({ .filePath = filePath, .access = StreamAccess::ReadOnly }, stream); result != PathResult::Success)
            return result;

        if (const StreamResult result = stream->ReadAll(outMemory); result != StreamResult::Success)
            return result;

        return std::nullopt;
    }

    /* --- POLLING METHODS --- */

    PathResult FileManager::ResolvePathConflict(const Path& path, const PathConflictPolicy conflictPolicy, Path& outResolvedPath) const
    {
        outResolvedPath = path;
        if (!PathExists(path))
            return PathResult::Success;

        switch (conflictPolicy)
        {
            case PathConflictPolicy::Overwrite:
            {
                return DeletePath(path);
            }
            case PathConflictPolicy::KeepExisting:
            {
                return PathResult::AlreadyExists;
            }
            case PathConflictPolicy::KeepBoth:
            {
                const std::string stem = path.GetStem();
                const std::string extension = path.GetExtension();

                uint32 copyIndex = 0;
                do
                {
                    outResolvedPath.ChangeFileName(SR_FORMAT("{0}_{1}{2}", stem, copyIndex, extension));
                    copyIndex++;
                }
                while (PathExists(outResolvedPath));
                break;
            }
        }

        return PathResult::Success;
    }

}
