//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "FileManager.h"

namespace Sierra
{

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
        return PathResult::Success;
    }

    PathResult FileManager::RenamePath(const Path& path, const std::string_view name) const
    {
        return PathResult::Success;
    }

    PathResult FileManager::MovePath(const Path& sourcePath, const Path& destinationDirectoryPath, const PathConflictPolicy conflictPolicy) const
    {
        return PathResult::Success;
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
        Path resolvedPath = { };
        if (const PathResult result = ResolvePathConflict(filePath, conflictPolicy, resolvedPath); result != PathResult::Success)
            return result;

        std::unique_ptr<FileStream> stream = nullptr;
        if (const PathResult result = OpenFileStream({ .filePath = resolvedPath, .access = StreamAccess::WriteOnly }, stream); result != PathResult::Success)
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
        const bool exists = PathExists(path);
        switch (conflictPolicy)
        {
            case PathConflictPolicy::Overwrite:
            {
                if (exists)
                {
                    if (const PathResult result = DeletePath(path); result != PathResult::Success)
                    {
                        outResolvedPath = path;
                        return result;
                    }
                }
                break;
            }
            case PathConflictPolicy::KeepExisting:
            {
                if (exists)
                {
                    return PathResult::AlreadyExists;
                }
            }
            case PathConflictPolicy::KeepBoth:
            {
                if (exists)
                {
                    const std::string stem = path.GetStem();

                    uint32 copyIndex = 0;
                    do
                    {
                        outResolvedPath.ChangeFileName(SR_FORMAT("{0}_{1}", stem, copyIndex));
                        copyIndex++;
                    }
                    while (PathExists(outResolvedPath));
                }
                break;
            }
        }

        return PathResult::Success;
    }

}