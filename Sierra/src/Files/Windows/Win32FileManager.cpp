//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#include "Win32FileManager.h"

#include <ShlObj_core.h>

#include "../PathErrors.h"
#include "Win32FileStream.h"
#include "Win32FileErrorHandler.h"

namespace Sierra
{


    /* --- POLLING METHODS --- */

    bool Win32FileManager::FileExists(const std::filesystem::path& filePath) const noexcept
    {
        const std::wstring path = filePath.wstring();
        const DWORD attributes = GetFileAttributesW(path.c_str());
        return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    std::unique_ptr<FileStream> Win32FileManager::OpenFileStream(const std::filesystem::path& filePath, const FileStreamAccess access, const FileStreamBuffering buffering) const
    {
        SR_THROW_IF(!FileExists(filePath), PathMissingError("Cannot open file stream, as the specified file path does not exist", filePath));

        DWORD fileSharing = 0;
        DWORD fileAccess = 0;
        switch (access)
        {
            case FileStreamAccess::ReadOnly:  { fileAccess = GENERIC_READ;                 fileSharing = FILE_SHARE_READ;                    break; }
            case FileStreamAccess::WriteOnly: { fileAccess = GENERIC_WRITE;                fileSharing = FILE_SHARE_WRITE;                   break; }
            case FileStreamAccess::ReadWrite: { fileAccess = GENERIC_READ | GENERIC_WRITE; fileSharing = FILE_SHARE_READ | FILE_SHARE_WRITE; break; }
        }

        const std::wstring path = filePath.wstring();

        std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> fileHandle = { CreateFileW(path.c_str(), fileAccess, fileSharing, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | (static_cast<uint8>(buffering == FileStreamBuffering::Unbuffered) * FILE_FLAG_NO_BUFFERING), nullptr), CloseHandle };
        if (fileHandle.get() == INVALID_HANDLE_VALUE) HandleWin32FileError(GetLastError(), "Could not open file stream", filePath);

        return std::make_unique<Win32FileStream>(fileHandle.release(), filePath);
    }

    void Win32FileManager::CreateFile(const std::filesystem::path& filePath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CreateFile(filePath, conflictPolicy);

        std::filesystem::path resolvedFilePath = filePath;
        ResolveFilePathConflict(filePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(resolvedFilePath.parent_path());

        const std::wstring path = resolvedFilePath.wstring();
        const std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> fileHandle = { CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr), CloseHandle };
        if (fileHandle.get() == INVALID_HANDLE_VALUE) HandleWin32FileError(GetLastError(), "Could create file", filePath);
    }

    void Win32FileManager::RenameFile(const std::filesystem::path& filePath, const std::string_view name) const
    {
        FileManager::RenameFile(filePath, name);

        const std::wstring sourcePath = filePath.wstring();
        const std::wstring movedPath = (filePath.parent_path() / name).wstring();

        if (!MoveFileW(sourcePath.c_str(), movedPath.c_str()))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not rename file to [{0}]", name), filePath);
        }
    }

    void Win32FileManager::CopyFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CopyFile(sourceFilePath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedFilePath = destinationDirectoryPath / sourceFilePath.filename();
        ResolveFilePathConflict(sourceFilePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        const std::wstring sourcePath = sourceFilePath.wstring();
        const std::wstring destinationPath = resolvedFilePath.wstring();

        if (!CopyFileW(sourcePath.c_str(), destinationPath.c_str(), conflictPolicy == FilePathConflictPolicy::KeepExisting))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not copy file to [{0}]", resolvedFilePath.string()), sourceFilePath);
        }
    }

    void Win32FileManager::MoveFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::MoveFile(sourceFilePath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedFilePath = destinationDirectoryPath / sourceFilePath.filename();
        ResolveFilePathConflict(sourceFilePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        const std::wstring sourcePath = sourceFilePath.wstring();
        const std::wstring destinationPath = resolvedFilePath.wstring();

        if (!MoveFileW(sourcePath.c_str(), destinationPath.c_str()))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not move file to [{0}]", resolvedFilePath.string()), sourceFilePath);
        }
    }

    void Win32FileManager::DeleteFile(const std::filesystem::path& filePath) const
    {
        FileManager::DeleteFile(filePath);
        const std::wstring path = filePath.wstring();

        if (!DeleteFileW(path.c_str()))
        {
            HandleWin32FileError(GetLastError(), "Could not delete file", filePath);
        }
    }

    bool Win32FileManager::DirectoryExists(const std::filesystem::path& directoryPath) const noexcept
    {
        const std::wstring path = directoryPath.wstring();
        const DWORD attributes = GetFileAttributesW(path.c_str());
        return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    void Win32FileManager::EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, bool recursive, const FileEnumerationPredicate& Predicate) const
    {
        FileManager::EnumerateDirectoryFiles(directoryPath, recursive, Predicate);

        if (!recursive)
        {
            for (const std::filesystem::path& path : std::filesystem::directory_iterator(directoryPath))
            {
                if (!std::filesystem::is_regular_file(path)) continue;
                Predicate(path);
            }
        }
        else
        {
            for (const std::filesystem::path& path : std::filesystem::recursive_directory_iterator(directoryPath))
            {
                if (!std::filesystem::is_regular_file(path)) continue;
                Predicate(path);
            }
        }
    }

    void Win32FileManager::CreateDirectory(const std::filesystem::path& directoryPath) const
    {
        FileManager::CreateDirectory(directoryPath);
        const std::wstring path = directoryPath.wstring();

        const DWORD errorCode = SHCreateDirectoryExW(nullptr, path.c_str(), nullptr);
        if (errorCode != ERROR_SUCCESS)
        {
            HandleWin32FileError(GetLastError(), "Could not create directory", directoryPath);
        }
    }

    void Win32FileManager::RenameDirectory(const std::filesystem::path& directoryPath, const std::string_view name) const
    {
        FileManager::RenameDirectory(directoryPath, name);

        const std::wstring sourcePath = directoryPath.wstring();
        const std::wstring movedPath = (directoryPath.parent_path() / name).wstring();

        if (!MoveFileW(sourcePath.c_str(), movedPath.c_str()))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not rename directory to [{0}]", name), directoryPath);
        }
    }

    void Win32FileManager::CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CopyDirectory(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedDirectoryPath = destinationDirectoryPath / sourceDirectoryPath.filename();
        ResolveDirectoryPathConflict(sourceDirectoryPath, resolvedDirectoryPath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        const std::wstring sourcePath = sourceDirectoryPath.wstring();
        const std::wstring destinationPath = resolvedDirectoryPath.wstring();

        if (!CopyFileW(sourcePath.c_str(), destinationPath.c_str(), conflictPolicy == FilePathConflictPolicy::KeepExisting))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not copy directory to [{0}]", resolvedDirectoryPath.string()), sourceDirectoryPath);
        }
    }

    void Win32FileManager::MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::MoveDirectory(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedDirectoryPath = destinationDirectoryPath / sourceDirectoryPath.filename();
        ResolveDirectoryPathConflict(sourceDirectoryPath, resolvedDirectoryPath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        const std::wstring sourcePath = sourceDirectoryPath.wstring();
        const std::wstring destinationPath = resolvedDirectoryPath.wstring();

        if (!MoveFileW(sourcePath.c_str(), destinationPath.c_str()))
        {
            HandleWin32FileError(GetLastError(), SR_FORMAT("Could not move directory to [{0}]", resolvedDirectoryPath.string()), sourceDirectoryPath);
        }
    }

    void Win32FileManager::DeleteDirectory(const std::filesystem::path& directoryPath) const
    {
        FileManager::DeleteDirectory(directoryPath);
        const std::wstring path = directoryPath.wstring();

        if (!DeleteFileW(path.c_str()))
        {
            HandleWin32FileError(GetLastError(), "Could not delete directory", directoryPath);
        }
    }

    /* --- GETTER METHODS --- */

    FileMetadata Win32FileManager::GetFileMetadata(const std::filesystem::path& filePath) const
    {
        const std::wstring path = filePath.wstring();

        const std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> fileHandle = { CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_READ_ATTRIBUTES, nullptr), CloseHandle };
        if (fileHandle.get() == INVALID_HANDLE_VALUE) HandleWin32FileError(GetLastError(), "Could not get file metadata", filePath);

        LARGE_INTEGER memorySize = { };
        GetFileSizeEx(fileHandle.get(), &memorySize);

        FILETIME creationFileTime = { };
        FILETIME lastWriteFileTime = { };
        GetFileTime(fileHandle.get(), &creationFileTime, nullptr, &lastWriteFileTime);

        FileTimeToLocalFileTime(&creationFileTime, &creationFileTime);
        FileTimeToLocalFileTime(&lastWriteFileTime, &lastWriteFileTime);

        SYSTEMTIME creationSystemTime = { };
        FileTimeToSystemTime(&creationFileTime, &creationSystemTime);

        SYSTEMTIME lastWriteSystemTime = { };
        FileTimeToSystemTime(&lastWriteFileTime, &lastWriteSystemTime);

        const Date dateCreated = Date({
            .second = static_cast<uint8>(creationSystemTime.wSecond),
            .minute = static_cast<uint8>(creationSystemTime.wMinute),
            .hour = static_cast<uint8>(creationSystemTime.wHour),
            .month = static_cast<Month>(creationSystemTime.wMonth - 1),
            .day = static_cast<uint8>(creationSystemTime.wDay),
            .year = creationSystemTime.wYear
        });

        const Date dateLastModified = Date({
            .second = static_cast<uint8>(lastWriteSystemTime.wSecond),
            .minute = static_cast<uint8>(lastWriteSystemTime.wMinute),
            .hour = static_cast<uint8>(lastWriteSystemTime.wHour),
            .month = static_cast<Month>(lastWriteSystemTime.wMonth - 1),
            .day = static_cast<uint8>(lastWriteSystemTime.wDay),
            .year = lastWriteSystemTime.wYear
        });

        return
        {
            .type = FilePathToFileType(filePath),
            .memorySize = static_cast<size>(memorySize.QuadPart),
            .dateCreated = dateCreated,
            .dateLastModified = dateLastModified
        };
    }

    DirectoryMetadata Win32FileManager::GetDirectoryMetadata(const std::filesystem::path& directoryPath) const
    {
        const std::wstring path = directoryPath.wstring();

        const std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> directoryHandle = { CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_READ_ATTRIBUTES, nullptr), CloseHandle };
        if (directoryHandle.get() == INVALID_HANDLE_VALUE)  HandleWin32FileError(GetLastError(), "Could not get directory metadata", directoryPath);

        FILETIME creationFileTime = { };
        FILETIME lastWriteFileTime = { };
        GetFileTime(directoryHandle.get(), &creationFileTime, nullptr, &lastWriteFileTime);

        FileTimeToLocalFileTime(&creationFileTime, &creationFileTime);
        FileTimeToLocalFileTime(&lastWriteFileTime, &lastWriteFileTime);

        SYSTEMTIME creationSystemTime = { };
        FileTimeToSystemTime(&creationFileTime, &creationSystemTime);

        SYSTEMTIME lastWriteSystemTime = { };
        FileTimeToSystemTime(&lastWriteFileTime, &lastWriteSystemTime);

        const Date dateCreated = Date({
            .second = static_cast<uint8>(creationSystemTime.wSecond),
            .minute = static_cast<uint8>(creationSystemTime.wMinute),
            .hour = static_cast<uint8>(creationSystemTime.wHour),
            .month = static_cast<Month>(creationSystemTime.wMonth - 1),
            .day = static_cast<uint8>(creationSystemTime.wDay),
            .year = creationSystemTime.wYear
        });

        const Date dateLastModified = Date({
            .second = static_cast<uint8>(lastWriteSystemTime.wSecond),
            .minute = static_cast<uint8>(lastWriteSystemTime.wMinute),
            .hour = static_cast<uint8>(lastWriteSystemTime.wHour),
            .month = static_cast<Month>(lastWriteSystemTime.wMonth - 1),
            .day = static_cast<uint8>(lastWriteSystemTime.wDay),
            .year = lastWriteSystemTime.wYear
        });

        uint32 fileCount = 0;
        size memorySize = 0;
        for (const std::filesystem::path& filePath : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            if (!std::filesystem::is_regular_file(filePath)) continue;
            memorySize += std::filesystem::file_size(filePath);
            fileCount++;
        }

        return
        {
            .fileCount = fileCount,
            .memorySize = memorySize,
            .dateCreated = dateCreated,
            .dateLastModified = dateLastModified
        };
    };

    std::filesystem::path Win32FileManager::GetApplicationDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath();
    }

    std::filesystem::path Win32FileManager::GetExecutableDirectoryPath() const noexcept
    {
        WCHAR path[MAX_PATH];
        const DWORD size = GetModuleFileNameW(nullptr, path, MAX_PATH);
        return std::filesystem::path(path, path + size).parent_path();
    }

    std::filesystem::path Win32FileManager::GetResourcesDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath() / "Resources";
    }

    std::filesystem::path Win32FileManager::GetHomeDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath().parent_path();
    }

    std::filesystem::path Win32FileManager::GetUserDirectoryPath() const noexcept
    {
        WCHAR path[MAX_PATH];
        const DWORD size = GetEnvironmentVariableW(L"USERPROFILE", path, MAX_PATH);
        return { path, path + size };
    }

    std::filesystem::path Win32FileManager::GetTemporaryDirectoryPath() const noexcept
    {
        WCHAR path[MAX_PATH];
        const DWORD size = GetTempPathW(MAX_PATH, path);
        return { path, path + size };
    }

    std::filesystem::path Win32FileManager::GetCachesDirectoryPath() const noexcept
    {
        return GetTemporaryDirectoryPath() / "Sierra Caches";
    }

    std::filesystem::path Win32FileManager::GetDesktopDirectoryPath() const noexcept
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Desktop directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetDownloadsDirectoryPath() const noexcept
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Downloads directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetDocumentsDirectoryPath() const noexcept
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Documents directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetPicturesDirectoryPath() const noexcept
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Pictures, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Pictures directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetMusicDirectoryPath() const noexcept
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Music, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Music directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetVideosDirectoryPath() const noexcept
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Videos, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        return folderPath.get();
    }

}