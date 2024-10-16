//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#include "Win32FileManager.h"

#include <ShlObj_core.h>

namespace Sierra
{

    namespace
    {
        FileOperationResult ErrorCodeToFileOperationResult(const UINT errorCode)
        {
            switch (errorCode)
            {
                case ERROR_SUCCESS:                    return FileOperationResult::Success;
                case ERROR_UNIDENTIFIED_ERROR:         return FileOperationResult::UnknownError;
                case ERROR_BAD_PATHNAME:               return FileOperationResult::FilePathInvalid;
                case ERROR_FILE_NOT_FOUND:             return FileOperationResult::NoSuchFilePath;
                case ERROR_FILE_EXISTS:                return FileOperationResult::FilePathAlreadyExists;
                case ERROR_ACCESS_DENIED:              return FileOperationResult::FileAccessDenied;
                case ERROR_SHARING_VIOLATION:          return FileOperationResult::FileLocked;
                case ERROR_FILE_CORRUPT:               return FileOperationResult::FileCorrupted;
                case ERROR_FILE_READ_ONLY:             return FileOperationResult::FileReadOnly;
                case ERROR_FILE_TOO_LARGE:             return FileOperationResult::FileTooLarge;
                default:                               break;
            }

            return FileOperationResult::UnknownError;
        }
    }

    /* --- CONSTRUCTORS --- */

    Win32FileStream::Win32FileStream(HANDLE fileHandle)
        : fileHandle(fileHandle)
    {
        SR_ERROR_IF(fileHandle == INVALID_HANDLE_VALUE, "File handle passed to Win32FileStream must not be [INVALID_HANDLE_VALUE]!");
    }

    /* --- POLLING METHODS --- */

    FileOperationResult Win32FileStream::Seek(const size offset)
    {
        const size currentOffset = GetCurrentOffset();
        if (const DWORD errorCode = SetFilePointerEx(fileHandle, { static_cast<DWORD>(offset - currentOffset) }, nullptr, FILE_CURRENT); errorCode == 0)
        {
            return ErrorCodeToFileOperationResult(GetLastError());
        }

        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileStream::Read(const size memorySize, std::vector<uint8>& outData)
    {
        outData = std::vector<uint8>(memorySize);
        if (!ReadFile(fileHandle, outData.data(), memorySize, nullptr, nullptr)) return ErrorCodeToFileOperationResult(GetLastError());
        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileStream::Write(const void* memory, const size memorySize, const size offset)
    {
        if (!WriteFile(fileHandle, reinterpret_cast<const uint8*>(memory) + offset, memorySize, nullptr, nullptr)) return ErrorCodeToFileOperationResult(GetLastError());
        return FileOperationResult::Success;
    }

    size Win32FileStream::GetMemorySize() const
    {
        LARGE_INTEGER memorySize = { };
        GetFileSizeEx(fileHandle, &memorySize);
        return memorySize.QuadPart;
    }

    size Win32FileStream::GetCurrentOffset() const
    {
        LARGE_INTEGER pointer = { };
        SetFilePointerEx(fileHandle, { 0 }, &pointer, FILE_CURRENT);
        return pointer.QuadPart;
    }

    /* --- DESTRUCTOR --- */

    Win32FileStream::~Win32FileStream()
    {
        CloseHandle(fileHandle);
    }

    /* --- POLLING METHODS --- */

    bool Win32FileManager::FileExists(const std::filesystem::path& filePath) const
    {
        const std::wstring path = filePath.wstring();
        const DWORD attributes = GetFileAttributesW(path.c_str());
        return attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    FileOperationResult Win32FileManager::OpenFileStream(const std::filesystem::path& filePath, FileStreamAccess access, const FileStreamBuffering buffering, std::unique_ptr<FileStream> &outFileStream) const
    {
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
        if (fileHandle.get() == INVALID_HANDLE_VALUE) return ErrorCodeToFileOperationResult(GetLastError());

        outFileStream = std::make_unique<Win32FileStream>(fileHandle.release());
        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::CreateFile(const std::filesystem::path& filePath) const
    {
        if (const FileOperationResult result = CreateDirectory(filePath.parent_path()); result != FileOperationResult::Success)
        {
            return result;
        }

        const std::wstring path = filePath.wstring();

        const std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> fileHandle = { CreateFileW(path.c_str(), GENERIC_WRITE, 0, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr), CloseHandle };
        if (fileHandle.get() == INVALID_HANDLE_VALUE) return ErrorCodeToFileOperationResult(GetLastError());

        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::RenameFile(const std::filesystem::path& filePath, const std::string_view name) const
    {
        const std::wstring sourcePath = filePath.wstring();
        const std::wstring movedPath = (filePath.parent_path() / name).wstring();

        if (!MoveFileW(sourcePath.c_str(), movedPath.c_str())) return ErrorCodeToFileOperationResult(GetLastError());
        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::CopyFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const
    {
        std::filesystem::path destinationFilePath = destinationDirectoryPath / sourceFilePath.filename();
        if (const FileOperationResult result = ResolveFilePathConflict(sourceFilePath, destinationFilePath, conflictPolicy); result != FileOperationResult::Success)
        {
            return result;
        }

        if (const FileOperationResult result = CreateDirectory(destinationDirectoryPath); result != FileOperationResult::Success)
        {
            return result;
        }

        const std::wstring sourcePath = sourceFilePath.wstring();
        const std::wstring destinationPath = destinationFilePath.wstring();

        if (!CopyFileW(sourcePath.c_str(), destinationPath.c_str(), conflictPolicy == FilePathConflictPolicy::KeepExisting)) return ErrorCodeToFileOperationResult(GetLastError());
        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::MoveFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const
    {
        std::filesystem::path destinationFilePath = destinationDirectoryPath / sourceFilePath.filename();
        if (const FileOperationResult result = ResolveFilePathConflict(sourceFilePath, destinationFilePath, conflictPolicy); result != FileOperationResult::Success)
        {
            return result;
        }

        if (const FileOperationResult result = CreateDirectory(destinationDirectoryPath); result != FileOperationResult::Success)
        {
            return result;
        }

        const std::wstring sourcePath = sourceFilePath.wstring();
        const std::wstring destinationPath = destinationFilePath.wstring();

        if (!MoveFileW(sourcePath.c_str(), destinationPath.c_str())) return ErrorCodeToFileOperationResult(GetLastError());
        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::DeleteFile(const std::filesystem::path& filePath) const
    {
        const std::wstring path = filePath.wstring();

        if (!DeleteFileW(path.c_str())) return ErrorCodeToFileOperationResult(GetLastError());
        return FileOperationResult::Success;
    }

    bool Win32FileManager::DirectoryExists(const std::filesystem::path& directoryPath) const
    {
        const std::wstring path = directoryPath.wstring();
        const DWORD attributes = GetFileAttributesW(path.c_str());
        return attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY);
    }

    FileOperationResult Win32FileManager::EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, std::vector<std::filesystem::path>& outFiles, bool recursive) const
    {
        if (!recursive)
        {
            for (const std::filesystem::path& path : std::filesystem::directory_iterator(directoryPath))
            {
                if (is_directory(path) || is_symlink(path)) continue;
                outFiles.emplace_back(path);
            }
        }
        else
        {
            for (const std::filesystem::path& path : std::filesystem::recursive_directory_iterator(directoryPath))
            {
                if (is_directory(path) || is_symlink(path)) continue;
                outFiles.emplace_back(path);
            }
        }

        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::CreateDirectory(const std::filesystem::path& directoryPath) const
    {
        if (DirectoryExists(directoryPath)) return FileOperationResult::Success;
        const std::wstring path = directoryPath.wstring();

        if (const DWORD errorCode = SHCreateDirectoryExW(nullptr, path.c_str(), nullptr); errorCode != ERROR_SUCCESS)
        {
            return ErrorCodeToFileOperationResult(errorCode);
        }
        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::RenameDirectory(const std::filesystem::path& directoryPath, const std::string_view name) const
    {
        return RenameFile(directoryPath, name);
    }

    FileOperationResult Win32FileManager::CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const
    {
        return CopyFile(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);
    }

    FileOperationResult Win32FileManager::MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const
    {
        return MoveFile(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);
    }

    FileOperationResult Win32FileManager::DeleteDirectory(const std::filesystem::path& directoryPath) const
    {
        return DeleteFile(directoryPath);
    }

    /* --- GETTER METHODS --- */

    FileOperationResult Win32FileManager::GetFileMetadata(const std::filesystem::path& filePath, FileMetadata& outMetadata) const
    {
        const std::wstring path = filePath.wstring();

        const std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> fileHandle = { CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_READ_ATTRIBUTES, nullptr), CloseHandle };
        if (fileHandle.get() == INVALID_HANDLE_VALUE) return ErrorCodeToFileOperationResult(GetLastError());

        LARGE_INTEGER memorySize = { };
        if (GetFileSizeEx(fileHandle.get(), &memorySize) == 0) return ErrorCodeToFileOperationResult(GetLastError());

        FILETIME creationFileTime = { };
        FILETIME lastWriteFileTime = { };
        if (GetFileTime(fileHandle.get(), &creationFileTime, nullptr, &lastWriteFileTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

        if (FileTimeToLocalFileTime(&creationFileTime, &creationFileTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());
        if (FileTimeToLocalFileTime(&lastWriteFileTime, &lastWriteFileTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

        SYSTEMTIME creationSystemTime = { };
        if (FileTimeToSystemTime(&creationFileTime, &creationSystemTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

        SYSTEMTIME lastWriteSystemTime = { };
        if (FileTimeToSystemTime(&lastWriteFileTime, &lastWriteSystemTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

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

        outMetadata = {
            .type = FilePathToFileType(filePath),
            .memorySize = static_cast<size>(memorySize.QuadPart),
            .dateCreated = dateCreated,
            .dateLastModified = dateLastModified
        };

        return FileOperationResult::Success;
    }

    FileOperationResult Win32FileManager::GetDirectoryMetadata(const std::filesystem::path& directoryPath, DirectoryMetadata& outMetadata) const
    {
        const std::wstring path = directoryPath.wstring();

        const std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> directoryHandle = { CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_READ_ATTRIBUTES, nullptr), CloseHandle };
        if (directoryHandle.get() == INVALID_HANDLE_VALUE) return ErrorCodeToFileOperationResult(GetLastError());

        FILETIME creationFileTime = { };
        FILETIME lastWriteFileTime = { };
        if (GetFileTime(directoryHandle.get(), &creationFileTime, nullptr, &lastWriteFileTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

        if (FileTimeToLocalFileTime(&creationFileTime, &creationFileTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());
        if (FileTimeToLocalFileTime(&lastWriteFileTime, &lastWriteFileTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

        SYSTEMTIME creationSystemTime = { };
        if (FileTimeToSystemTime(&creationFileTime, &creationSystemTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

        SYSTEMTIME lastWriteSystemTime = { };
        if (FileTimeToSystemTime(&lastWriteFileTime, &lastWriteSystemTime) == 0) return ErrorCodeToFileOperationResult(GetLastError());

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
        for (const std::filesystem::path& currentPath : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            const std::wstring filePath = currentPath.wstring();
            const DWORD fileAttributes = GetFileAttributesW(currentPath.c_str());

            if (fileAttributes == INVALID_FILE_ATTRIBUTES || (fileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
            fileCount++;

            const std::unique_ptr<std::remove_pointer_t<HANDLE>, std::function<void(HANDLE)>> fileHandle = { CreateFileW(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_READ_ATTRIBUTES, nullptr), CloseHandle };
            if (fileHandle.get() == INVALID_HANDLE_VALUE) continue;

            LARGE_INTEGER fileMemorySize = { };
            if (GetFileSizeEx(fileHandle.get(), &fileMemorySize) == 0) continue;

            memorySize += fileMemorySize.QuadPart;
        }

        outMetadata = {
            .fileCount = fileCount,
            .memorySize = memorySize,
            .dateCreated = dateCreated,
            .dateLastModified = dateLastModified
        };

        return FileOperationResult::Success;
    };

    std::filesystem::path Win32FileManager::GetApplicationDirectoryPath() const
    {
        return GetExecutableDirectoryPath();
    }

    std::filesystem::path Win32FileManager::GetExecutableDirectoryPath() const
    {
        WCHAR path[MAX_PATH];
        const DWORD size = GetModuleFileNameW(nullptr, path, MAX_PATH);
        return std::filesystem::path(path, path + size).parent_path();
    }

    std::filesystem::path Win32FileManager::GetResourcesDirectoryPath() const
    {
        return GetExecutableDirectoryPath() / "Resources";
    }

    std::filesystem::path Win32FileManager::GetHomeDirectoryPath() const
    {
        return GetUserDirectoryPath().parent_path();
    }

    std::filesystem::path Win32FileManager::GetUserDirectoryPath() const
    {
        WCHAR path[MAX_PATH];
        const DWORD size = GetEnvironmentVariableW(L"USERPROFILE", path, MAX_PATH);
        return { path, path + size };
    }

    std::filesystem::path Win32FileManager::GetTemporaryDirectoryPath() const
    {
        WCHAR path[MAX_PATH];
        const DWORD size = GetTempPathW(MAX_PATH, path);
        return { path, path + size };
    }

    std::filesystem::path Win32FileManager::GetCachesDirectoryPath() const
    {
        return GetTemporaryDirectoryPath() / "Sierra Caches";
    }

    std::filesystem::path Win32FileManager::GetDesktopDirectoryPath() const
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Desktop, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Desktop directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetDownloadsDirectoryPath() const
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Downloads, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Downloads directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetDocumentsDirectoryPath() const
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Documents directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetPicturesDirectoryPath() const
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Pictures, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Pictures directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetMusicDirectoryPath() const
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Music, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Music directory path!");

        return folderPath.get();
    }

    std::filesystem::path Win32FileManager::GetVideosDirectoryPath() const
    {
        WCHAR* path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_Videos, KF_FLAG_CREATE, nullptr, &path);

        const std::unique_ptr<WCHAR, std::function<void(LPVOID)>> folderPath = { path, CoTaskMemFree };
        SR_ERROR_IF(!SUCCEEDED(result), "Could not retrieve Videos directory path!");

        return folderPath.get();
    }

}