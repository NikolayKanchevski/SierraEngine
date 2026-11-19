//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#include "Win32FileManager.h"

#if !defined(WIN32_LEAN_AND_MEAN)
    #define WIN32_LEAN_AND_MEAN
#endif
#if !defined(NOMINMAX)
    #define NOMINMAX
#endif
#include <Windows.h>
#include <ShlObj.h>

#pragma region Windows Macros // Windows.h defines macros, which collide with names of the methods implemented here, so they are removed
    #undef CreateFile
    #undef CreateDirectory
    #undef CopyFile
    #undef MoveFile
    #undef DeleteFile
#pragma endregion

#include "Win32FileStream.h"

namespace Sierra
{

    namespace
    {
        [[nodiscard]] PathResult Win32ErrorToPathResult(const DWORD errorCode) noexcept
        {
            switch (errorCode)
            {
                case ERROR_SUCCESS:                 return PathResult::Success;
                case ERROR_FILE_NOT_FOUND:
                case ERROR_PATH_NOT_FOUND:
                case ERROR_INVALID_DRIVE:
                case ERROR_BAD_NETPATH:
                case ERROR_DIRECTORY:               return PathResult::NotFound;
                case ERROR_FILE_EXISTS:
                case ERROR_ALREADY_EXISTS:          return PathResult::AlreadyExists;
                case ERROR_ACCESS_DENIED:
                case ERROR_PRIVILEGE_NOT_HELD:      return PathResult::PermissionDenied;
                case ERROR_WRITE_PROTECT:
                case ERROR_FILE_READ_ONLY:          return PathResult::ReadOnly;
                case ERROR_FILE_CORRUPT:
                case ERROR_DISK_CORRUPT:            return PathResult::Corrupted;
                case ERROR_SHARING_VIOLATION:
                case ERROR_LOCK_VIOLATION:
                case ERROR_BUSY:                    return PathResult::Locked;
                case ERROR_DISK_FULL:
                case ERROR_HANDLE_DISK_FULL:
                case ERROR_FILE_TOO_LARGE:          return PathResult::SizeExceeded;
                default:                            break;
            }

            return PathResult::Unknown;
        }

        // Errors of the standard library are reported as Win32 errors by some implementations, and as errno values by others
        [[nodiscard]] PathResult ErrorCodeToPathResult(const std::error_code& errorCode) noexcept
        {
            if (!errorCode) return PathResult::Success;
            if (errorCode.category() == std::system_category()) return Win32ErrorToPathResult(static_cast<DWORD>(errorCode.value()));

            const std::error_condition condition = errorCode.default_error_condition();
            if (condition == std::errc::no_such_file_or_directory || condition == std::errc::not_a_directory) return PathResult::NotFound;
            if (condition == std::errc::file_exists) return PathResult::AlreadyExists;
            if (condition == std::errc::permission_denied || condition == std::errc::operation_not_permitted) return PathResult::PermissionDenied;
            if (condition == std::errc::read_only_file_system) return PathResult::ReadOnly;
            if (condition == std::errc::device_or_resource_busy || condition == std::errc::text_file_busy) return PathResult::Locked;
            if (condition == std::errc::file_too_large || condition == std::errc::no_space_on_device) return PathResult::SizeExceeded;

            return PathResult::Unknown;
        }

        [[nodiscard]] TimePoint FileTimeToTimePoint(const FILETIME fileTime) noexcept
        {
            // File times count intervals of 100 nanoseconds since 1601, while the system clock counts from 1970
            constexpr int64 UNIX_EPOCH_OFFSET = 116'444'736'000'000'000;
            const int64 intervals = static_cast<int64>((static_cast<uint64>(fileTime.dwHighDateTime) << 32) | fileTime.dwLowDateTime);

            using FileTimeDuration = std::chrono::duration<int64, std::ratio<1, 10'000'000>>;
            return TimePoint(std::chrono::system_clock::time_point(std::chrono::duration_cast<std::chrono::system_clock::duration>(FileTimeDuration(intervals - UNIX_EPOCH_OFFSET))));
        }

        // Paths are passed to Windows as UTF-16, which is what they are natively stored as
        [[nodiscard]] const wchar_t* GetNativePath(const Path& path) noexcept
        {
            return path.GetStandardPath().c_str();
        }

        [[nodiscard]] DWORD GetPathAttributes(const Path& path) noexcept
        {
            return GetFileAttributesW(GetNativePath(path));
        }

        [[nodiscard]] Path GetKnownFolderPath(const KNOWNFOLDERID& folderID) noexcept
        {
            PWSTR folderPath = nullptr;
            const HRESULT result = SHGetKnownFolderPath(folderID, KF_FLAG_DEFAULT, nullptr, &folderPath);

            Path path = { };
            if (SUCCEEDED(result) && folderPath != nullptr) path = std::filesystem::path(folderPath);

            // NOTE: Memory must be freed even if retrieving the path failed
            CoTaskMemFree(folderPath);
            return path;
        }

        [[nodiscard]] PathResult CreateDirectories(const Path& directoryPath)
        {
            if (directoryPath.IsEmpty())
                return PathResult::Success;

            std::error_code errorCode = { };
            std::filesystem::create_directories(directoryPath.GetStandardPath(), errorCode);
            return ErrorCodeToPathResult(errorCode);
        }

        [[nodiscard]] PathResult CopyPathContents(const Path& sourcePath, const Path& destinationPath)
        {
            std::error_code errorCode = { };
            std::filesystem::copy(sourcePath.GetStandardPath(), destinationPath.GetStandardPath(), std::filesystem::copy_options::recursive, errorCode);
            return ErrorCodeToPathResult(errorCode);
        }

        [[nodiscard]] DWORD StreamAccessToDesiredAccess(const StreamAccess access) noexcept
        {
            switch (access)
            {
                case StreamAccess::ReadOnly:      return GENERIC_READ;
                case StreamAccess::WriteOnly:     return GENERIC_WRITE;
                case StreamAccess::ReadWrite:     return GENERIC_READ | GENERIC_WRITE;
            }

            return 0;
        }

        [[nodiscard]] DWORD FileStreamBufferingToFlags(const FileStreamBuffering buffering) noexcept
        {
            // NOTE: FILE_FLAG_NO_BUFFERING is not used, as it only allows transfers of whole sectors to and from aligned memory, which streams cannot guarantee. Writes still go straight to the disk, as they do on the other platforms
            return buffering == FileStreamBuffering::Unbuffered ? FILE_FLAG_WRITE_THROUGH : 0;
        }
    }

    /* --- POLLING METHODS --- */

    bool Win32FileManager::PathExists(const Path& path) const noexcept
    {
        return GetPathAttributes(path) != INVALID_FILE_ATTRIBUTES;
    }

    PathResult Win32FileManager::CreateFile(const Path& filePath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CreateFile(filePath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedFilePath = filePath;
        if (const PathResult result = ResolvePathConflict(filePath, conflictPolicy, resolvedFilePath); result != PathResult::Success)
            return result;

        // NOTE: Parent directories are created without a conflict policy, as resolving one would delete them (along with their contents) when they already exist
        if (const PathResult result = CreateDirectories(resolvedFilePath.GetParentPath()); result != PathResult::Success)
            return result;

        // NOTE: An existing file is opened as it is (without being emptied), as on the other platforms
        const HANDLE fileHandle = CreateFileW(GetNativePath(resolvedFilePath), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (fileHandle == INVALID_HANDLE_VALUE)
            return Win32ErrorToPathResult(GetLastError());

        CloseHandle(fileHandle);
        return PathResult::Success;
    }

    PathResult Win32FileManager::CreateDirectory(const Path& directoryPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CreateDirectory(directoryPath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedDirectoryPath = directoryPath;
        if (const PathResult result = ResolvePathConflict(directoryPath, conflictPolicy, resolvedDirectoryPath); result != PathResult::Success)
            return result;

        return CreateDirectories(resolvedDirectoryPath);
    }

    PathResult Win32FileManager::CopyPath(const Path& sourcePath, const Path& destinationPath, const PathConflictPolicy conflictPolicy) const
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

    PathResult Win32FileManager::RenamePath(const Path& path, const std::string_view name) const
    {
        // NOTE: Base implementation makes sure nothing exists under the new name
        if (const PathResult result = FileManager::RenamePath(path, name); result != PathResult::Success)
            return result;

        const Path renamedPath = path.GetParentPath() / name;
        if (!MoveFileExW(GetNativePath(path), GetNativePath(renamedPath), 0))
            return Win32ErrorToPathResult(GetLastError());

        return PathResult::Success;
    }

    PathResult Win32FileManager::MovePath(const Path& sourcePath, const Path& destinationPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::MovePath(sourcePath, destinationPath, conflictPolicy); result != PathResult::Success)
            return result;

        const Path destinationItemPath = destinationPath / sourcePath.GetFileName();

        Path resolvedPath = destinationItemPath;
        if (const PathResult result = ResolvePathConflict(destinationItemPath, conflictPolicy, resolvedPath); result != PathResult::Success)
            return result;

        if (const PathResult result = CreateDirectories(destinationPath); result != PathResult::Success)
            return result;

        // NOTE: Files are copied over to other volumes by Windows itself, but directories are not
        if (MoveFileExW(GetNativePath(sourcePath), GetNativePath(resolvedPath), MOVEFILE_COPY_ALLOWED))
            return PathResult::Success;

        if (const DWORD errorCode = GetLastError(); errorCode != ERROR_NOT_SAME_DEVICE)
            return Win32ErrorToPathResult(errorCode);

        // Directories cannot be moved across volumes, so they are copied over and then deleted instead
        if (const PathResult result = CopyPathContents(sourcePath, resolvedPath); result != PathResult::Success)
            return result;

        return DeletePath(sourcePath);
    }

    PathResult Win32FileManager::DeletePath(const Path& path) const
    {
        if (const PathResult result = FileManager::DeletePath(path); result != PathResult::Success)
            return result;

        std::error_code errorCode = { };
        std::filesystem::remove_all(path.GetStandardPath(), errorCode);

        return ErrorCodeToPathResult(errorCode);
    }

    PathResult Win32FileManager::OpenFileStream(const FileStreamCreateInfo& createInfo, std::unique_ptr<FileStream>& outFileStream) const
    {
        if (!IsFile(createInfo.filePath))
            return PathResult::NotFound;

        // NOTE: Files are shared with every other stream (and process), as files on the other platforms are not locked either
        const HANDLE fileHandle = CreateFileW(GetNativePath(createInfo.filePath), StreamAccessToDesiredAccess(createInfo.access), FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FileStreamBufferingToFlags(createInfo.buffering), nullptr);
        if (fileHandle == INVALID_HANDLE_VALUE)
            return Win32ErrorToPathResult(GetLastError());

        outFileStream = std::make_unique<Win32FileStream>(fileHandle, createInfo);
        return PathResult::Success;
    }

    PathResult Win32FileManager::EnumeratePaths(const Path& path, const PathEnumerationPredicate& Predicate) const
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

        while (!errorCode && iterator != end)
        {
            if (Predicate(iterator->path()))
            {
                iterator.disable_recursion_pending();
            }

            iterator.increment(errorCode);
        }

        return ErrorCodeToPathResult(errorCode);
    }

    /* --- GETTER METHODS --- */

    bool Win32FileManager::IsFile(const Path& path) const
    {
        const DWORD attributes = GetPathAttributes(path);
        return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    bool Win32FileManager::IsDirectory(const Path& path) const
    {
        const DWORD attributes = GetPathAttributes(path);
        return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
    }

    PathResult Win32FileManager::GetPathMetadata(const Path& path, PathMetadata& outMetadata) const
    {
        WIN32_FILE_ATTRIBUTE_DATA attributeData = { };
        if (!GetFileAttributesExW(GetNativePath(path), GetFileExInfoStandard, &attributeData))
            return Win32ErrorToPathResult(GetLastError());

        outMetadata =
        {
            .itemCount = 1,
            .memorySize = static_cast<size>((static_cast<uint64>(attributeData.nFileSizeHigh) << 32) | attributeData.nFileSizeLow),
            .timeCreated = FileTimeToTimePoint(attributeData.ftCreationTime),
            .timeLastModified = FileTimeToTimePoint(attributeData.ftLastWriteTime)
        };

        if ((attributeData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            uint32 itemCount = 0;

            std::error_code errorCode = { };
            std::filesystem::directory_iterator iterator(path.GetStandardPath(), errorCode);
            const std::filesystem::directory_iterator end;

            for (; !errorCode && iterator != end; iterator.increment(errorCode))
            {
                itemCount++;
            }

            outMetadata.itemCount = itemCount;
        }

        return PathResult::Success;
    }

    Path Win32FileManager::GetApplicationDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath();
    }

    Path Win32FileManager::GetExecutableDirectoryPath() const noexcept
    {
        // Path of the executable may exceed MAX_PATH characters, in which case it gets truncated, and the buffer is grown until it fits
        std::wstring executablePath(MAX_PATH, L'\0');
        while (true)
        {
            const DWORD length = GetModuleFileNameW(nullptr, executablePath.data(), static_cast<DWORD>(executablePath.size()));
            if (length == 0) return { };

            if (length < executablePath.size())
            {
                executablePath.resize(length);
                break;
            }

            executablePath.resize(executablePath.size() * 2);
        }

        return std::filesystem::path(executablePath).parent_path();
    }

    Path Win32FileManager::GetResourcesDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath() / "Resources";
    }

    Path Win32FileManager::GetHomeDirectoryPath() const noexcept
    {
        // Directory, which contains the directories of all users (usually C:\Users)
        if (Path path = GetKnownFolderPath(FOLDERID_UserProfiles); !path.IsEmpty())
            return path;

        return GetUserDirectoryPath().GetParentPath();
    }

    Path Win32FileManager::GetUserDirectoryPath() const noexcept
    {
        if (Path path = GetKnownFolderPath(FOLDERID_Profile); !path.IsEmpty())
            return path;

        std::wstring userProfile(MAX_PATH, L'\0');
        const DWORD length = GetEnvironmentVariableW(L"USERPROFILE", userProfile.data(), static_cast<DWORD>(userProfile.size()));
        if (length == 0 || length >= userProfile.size())
            return { };

        userProfile.resize(length);
        return std::filesystem::path(userProfile);
    }

    Path Win32FileManager::GetTemporaryDirectoryPath() const noexcept
    {
        std::error_code errorCode = { };
        return std::filesystem::temp_directory_path(errorCode);
    }

    Path Win32FileManager::GetCachesDirectoryPath() const noexcept
    {
        // Local application data (usually C:\Users\<User>\AppData\Local) is what holds caches of applications on Windows
        if (Path path = GetKnownFolderPath(FOLDERID_LocalAppData); !path.IsEmpty())
            return path;

        return GetTemporaryDirectoryPath();
    }

    Path Win32FileManager::GetDesktopDirectoryPath() const noexcept
    {
        return GetKnownFolderPath(FOLDERID_Desktop);
    }

    Path Win32FileManager::GetDownloadsDirectoryPath() const noexcept
    {
        return GetKnownFolderPath(FOLDERID_Downloads);
    }

    Path Win32FileManager::GetDocumentsDirectoryPath() const noexcept
    {
        return GetKnownFolderPath(FOLDERID_Documents);
    }

    Path Win32FileManager::GetPicturesDirectoryPath() const noexcept
    {
        return GetKnownFolderPath(FOLDERID_Pictures);
    }

    Path Win32FileManager::GetMusicDirectoryPath() const noexcept
    {
        return GetKnownFolderPath(FOLDERID_Music);
    }

    Path Win32FileManager::GetVideosDirectoryPath() const noexcept
    {
        return GetKnownFolderPath(FOLDERID_Videos);
    }

}
