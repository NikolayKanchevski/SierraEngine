//
// Created by Nikolay Kanchevski on 9.19.24.
//

#include "UnixFileManager.h"

#include <fcntl.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/stat.h>

#include "UnixFileStream.h"

namespace Sierra
{

    namespace
    {
        [[nodiscard]] PathResult ErrnoToPathResult(const int errorCode) noexcept
        {
            switch (errorCode)
            {
                case 0:                     return PathResult::Success;
                case ENOENT:
                case ENOTDIR:               return PathResult::NotFound;
                case EEXIST:                return PathResult::AlreadyExists;
                case EACCES:
                case EPERM:                 return PathResult::PermissionDenied;
                case EROFS:                 return PathResult::ReadOnly;
                case ETXTBSY:
                case EBUSY:                 return PathResult::Locked;
                case EFBIG:
                case ENOSPC:
                case EDQUOT:                return PathResult::SizeExceeded;
                default:                    break;
            }

            return PathResult::Unknown;
        }

        [[nodiscard]] int StreamAccessToFlags(const StreamAccess access) noexcept
        {
            int flags = 0;
            switch (access)
            {
                case StreamAccess::ReadOnly:      { flags |= O_RDONLY; break; }
                case StreamAccess::WriteOnly:     { flags |= O_WRONLY; break; }
                case StreamAccess::ReadWrite:     { flags |= O_RDWR;   break; }
            }

            return flags;
        }

        [[nodiscard]] int FileStreamBufferingToFlags(const FileStreamBuffering buffering) noexcept
        {
            return buffering == FileStreamBuffering::Unbuffered ? O_SYNC | O_DIRECT : 0;
        }

        [[nodiscard]] PathResult CreateDirectories(const Path& directoryPath)
        {
            if (directoryPath.IsEmpty())
                return PathResult::Success;

            std::error_code errorCode = { };
            std::filesystem::create_directories(directoryPath.GetStandardPath(), errorCode);
            return ErrnoToPathResult(errorCode.value());
        }

        [[nodiscard]] PathResult CopyPathContents(const Path& sourcePath, const Path& destinationPath)
        {
            std::error_code errorCode = { };
            std::filesystem::copy(sourcePath.GetStandardPath(), destinationPath.GetStandardPath(), std::filesystem::copy_options::recursive, errorCode);
            return ErrnoToPathResult(errorCode.value());
        }
    }

    /* --- POLLING METHODS --- */

    bool UnixFileManager::PathExists(const Path& path) const noexcept
    {
        const std::string pathString = path.GetString();

        struct stat status = { };
        return stat(pathString.c_str(), &status) == 0;
    }

    PathResult UnixFileManager::CreateFile(const Path& filePath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CreateFile(filePath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedFilePath = filePath;
        if (const PathResult result = ResolvePathConflict(filePath, conflictPolicy, resolvedFilePath); result != PathResult::Success)
            return result;

        // NOTE: Parent directories are created without a conflict policy, as resolving one would delete them (along with their contents) when they already exist
        if (const PathResult result = CreateDirectories(resolvedFilePath.GetParentPath()); result != PathResult::Success)
            return result;

        const std::string pathString = resolvedFilePath.GetString();
        const int fileDescriptor = open(pathString.c_str(), O_CREAT | O_WRONLY | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);

        if (fileDescriptor == -1)
            return ErrnoToPathResult(errno);

        close(fileDescriptor);
        return PathResult::Success;
    }

    PathResult UnixFileManager::CreateDirectory(const Path& directoryPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CreateDirectory(directoryPath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedDirectoryPath = directoryPath;
        if (const PathResult result = ResolvePathConflict(directoryPath, conflictPolicy, resolvedDirectoryPath); result != PathResult::Success)
            return result;

        return CreateDirectories(resolvedDirectoryPath);
    }

    PathResult UnixFileManager::CopyPath(const Path& sourcePath, const Path& destinationPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CopyPath(sourcePath, destinationPath, conflictPolicy); result != PathResult::Success)
            return result;

        const Path destinationItemPath = destinationPath / sourcePath.GetFileName();

        Path resolvedPath = destinationItemPath;
        if (const PathResult result = ResolvePathConflict(destinationItemPath, conflictPolicy, resolvedPath); result != PathResult::Success)
            return result;

        if (const PathResult result = CreateDirectories(destinationPath); result != PathResult::Success)
            return result;

        return CopyPathContents(sourcePath, resolvedPath);
    }

    PathResult UnixFileManager::RenamePath(const Path& path, const std::string_view name) const
    {
        // NOTE: Base implementation makes sure nothing exists under the new name, as rename() would otherwise silently replace it
        if (const PathResult result = FileManager::RenamePath(path, name); result != PathResult::Success)
            return result;

        const std::string sourcePathString = path.GetString();
        const std::string renamedPathString = (path.GetParentPath() / name).GetString();

        if (rename(sourcePathString.c_str(), renamedPathString.c_str()) != 0)
            return ErrnoToPathResult(errno);

        return PathResult::Success;
    }

    PathResult UnixFileManager::MovePath(const Path& sourcePath, const Path& destinationPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::MovePath(sourcePath, destinationPath, conflictPolicy); result != PathResult::Success)
            return result;

        const Path destinationItemPath = destinationPath / sourcePath.GetFileName();

        Path resolvedPath = destinationItemPath;
        if (const PathResult result = ResolvePathConflict(destinationItemPath, conflictPolicy, resolvedPath); result != PathResult::Success)
            return result;

        if (const PathResult result = CreateDirectories(destinationPath); result != PathResult::Success)
            return result;

        const std::string sourcePathString = sourcePath.GetString();
        const std::string resolvedPathString = resolvedPath.GetString();

        if (rename(sourcePathString.c_str(), resolvedPathString.c_str()) == 0)
            return PathResult::Success;

        if (errno != EXDEV)
            return ErrnoToPathResult(errno);

        // Paths cannot be renamed across file systems, so they are copied over and then deleted instead
        if (const PathResult result = CopyPathContents(sourcePath, resolvedPath); result != PathResult::Success)
            return result;

        return DeletePath(sourcePath);
    }

    PathResult UnixFileManager::DeletePath(const Path& path) const
    {
        if (const PathResult result = FileManager::DeletePath(path); result != PathResult::Success)
            return result;

        std::error_code errorCode = { };
        std::filesystem::remove_all(path.GetStandardPath(), errorCode);

        return ErrnoToPathResult(errorCode.value());
    }

    PathResult UnixFileManager::OpenFileStream(const FileStreamCreateInfo& createInfo, std::unique_ptr<FileStream>& outFileStream) const
    {
        if (!IsFile(createInfo.filePath))
            return PathResult::NotFound;

        const std::string pathString = createInfo.filePath.GetString();
        const int fileDescriptor = open(pathString.c_str(), StreamAccessToFlags(createInfo.access) | FileStreamBufferingToFlags(createInfo.buffering) | O_CLOEXEC);

        if (fileDescriptor == -1)
            return ErrnoToPathResult(errno);

        outFileStream = std::make_unique<UnixFileStream>(fileDescriptor, createInfo);
        return PathResult::Success;
    }

    PathResult UnixFileManager::EnumeratePaths(const Path& path, const PathEnumerationPredicate& Predicate) const
    {
        if (const PathResult result = FileManager::EnumeratePaths(path, Predicate); result != PathResult::Success)
            return result;

        if (!IsDirectory(path))
        {
            Predicate(path);
            return PathResult::Success;
        }

        std::error_code errorCode = { };
        std::filesystem::recursive_directory_iterator iterator(path.GetStandardPath(), std::filesystem::directory_options::skip_permission_denied, errorCode);
        const std::filesystem::recursive_directory_iterator end;

        while (errorCode.value() == 0 && iterator != end)
        {
            if (Predicate(iterator->path()))
            {
                iterator.disable_recursion_pending();
            }

            iterator.increment(errorCode);
        }

        return ErrnoToPathResult(errorCode.value());
    }

    /* --- GETTER METHODS --- */

    bool UnixFileManager::IsFile(const Path& path) const
    {
        const std::string pathString = path.GetString();

        struct stat status = { };
        return stat(pathString.c_str(), &status) == 0 && S_ISREG(status.st_mode);
    }

    bool UnixFileManager::IsDirectory(const Path& path) const
    {
        const std::string pathString = path.GetString();

        struct stat status = { };
        return stat(pathString.c_str(), &status) == 0 && S_ISDIR(status.st_mode);
    }

    PathResult UnixFileManager::GetPathMetadata(const Path& path, PathMetadata& outMetadata) const
    {
        const std::string pathString = path.GetString();

        struct stat status = { };
        if (stat(pathString.c_str(), &status) != 0)
            return ErrnoToPathResult(errno);

        outMetadata =
        {
            .itemCount = 1,
            .memorySize = static_cast<size>(status.st_size),
            .timeCreated = TimePoint(std::chrono::system_clock::from_time_t(status.st_ctime)),
            .timeLastModified = TimePoint(std::chrono::system_clock::from_time_t(status.st_mtime))
        };

        if (S_ISDIR(status.st_mode))
        {
            uint32 itemCount = 0;

            std::error_code errorCode = { };
            std::filesystem::directory_iterator iterator(path.GetStandardPath(), errorCode);
            const std::filesystem::directory_iterator end;

            for (; errorCode.value() == 0 && iterator != end; iterator.increment(errorCode))
            {
                itemCount++;
            }

            outMetadata.itemCount = itemCount;
        }

        return PathResult::Success;
    }

    Path UnixFileManager::GetApplicationDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath();
    }

    Path UnixFileManager::GetExecutableDirectoryPath() const noexcept
    {
        std::error_code errorCode = { };
        const std::filesystem::path executablePath = std::filesystem::read_symlink("/proc/self/exe", errorCode);
        return executablePath.parent_path();
    }

    Path UnixFileManager::GetResourcesDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath() / "Resources";
    }

    Path UnixFileManager::GetHomeDirectoryPath() const noexcept
    {
        return "/home";
    }

    Path UnixFileManager::GetUserDirectoryPath() const noexcept
    {
        if (const char* const homePath = std::getenv("HOME"); homePath != nullptr)
            return homePath;

        if (const passwd* const user = getpwuid(getuid()); user != nullptr)
            return user->pw_dir;

        return { };
    }

    Path UnixFileManager::GetTemporaryDirectoryPath() const noexcept
    {
        std::error_code errorCode = { };
        return std::filesystem::temp_directory_path(errorCode);
    }

    Path UnixFileManager::GetCachesDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / ".cache";
    }

    Path UnixFileManager::GetDesktopDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Desktop";
    }

    Path UnixFileManager::GetDownloadsDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Downloads";
    }

    Path UnixFileManager::GetDocumentsDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Documents";
    }

    Path UnixFileManager::GetPicturesDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Pictures";
    }

    Path UnixFileManager::GetMusicDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Music";
    }

    Path UnixFileManager::GetVideosDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Videos";
    }

}
