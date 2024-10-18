//
// Created by Nikolay Kanchevski on 9.19.24.
//

#include "UnixFileManager.h"

#include <fcntl.h>
#include <sys/stat.h>

#include "../PathErrors.h"
#include "UnixFileStream.h"
#include "UnixFileErrorHandler.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    bool UnixFileManager::FileExists(const std::filesystem::path& filePath) const noexcept
    {
        const std::string path = filePath.string();

        struct stat buffer = { };
        return (stat(path.c_str(), &buffer) == 0);
    }

    std::unique_ptr<FileStream> UnixFileManager::OpenFileStream(const std::filesystem::path& filePath, const FileStreamAccess access, const FileStreamBuffering buffering) const
    {
        SR_THROW_IF(!FileExists(filePath), PathMissingError("Cannot open file stream, as the specified file path does not exist", filePath));

        const std::string path = filePath.string();

        int flags = static_cast<uint8>(buffering == FileStreamBuffering::Unbuffered) * (O_SYNC | O_DIRECT);
        switch (access)
        {
            case FileStreamAccess::ReadOnly:  { flags |= O_RDONLY; break; }
            case FileStreamAccess::WriteOnly: { flags |= O_WRONLY; break; }
            case FileStreamAccess::ReadWrite: { flags |= O_RDWR;   break; }
            default:                          break;
        }

        const int fileDescriptor = open(path.c_str(), flags);
        if (fileDescriptor == -1) HandleUnixFileError(errno, "Could not open file stream", filePath);

        return std::make_unique<UnixFileStream>(fileDescriptor, filePath);
    }

    void UnixFileManager::CreateFile(const std::filesystem::path& filePath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CreateFile(filePath, conflictPolicy);

        std::filesystem::path resolvedFilePath = filePath;
        ResolveFilePathConflict(filePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(resolvedFilePath.parent_path());

        const std::string path = resolvedFilePath.string();
        const int fileDescriptor = open(path.c_str(), O_CREAT | O_WRONLY, S_IRWXU);

        if (fileDescriptor == -1) HandleUnixFileError(errno, "Could not create file", filePath);
        close(fileDescriptor);
    }

    void UnixFileManager::RenameFile(const std::filesystem::path& filePath, const std::string_view name) const
    {
        FileManager::RenameFile(filePath, name);

        const std::string sourcePath = filePath.string();
        const std::string renamedPath = (filePath.parent_path() / name).string();

        if (rename(sourcePath.c_str(), renamedPath.c_str()) != 0)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not rename file to [{0}]", name), filePath);
        }
    }

    void UnixFileManager::CopyFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CopyFile(sourceFilePath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedFilePath = destinationDirectoryPath / sourceFilePath.filename();
        ResolveFilePathConflict(sourceFilePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        const std::string sourcePath = sourceFilePath.string();
        const std::string destinationPath = resolvedFilePath.string();

        const int sourceFileDescriptor = open(sourcePath.c_str(), O_RDONLY);
        if (sourceFileDescriptor == -1) HandleUnixFileError(errno, "Could not open file to copy from", sourceFilePath);

        const int destinationFileDescriptor = open(destinationPath.c_str(), O_CREAT | O_WRONLY, S_IRWXU);
        if (destinationFileDescriptor == -1) HandleUnixFileError(errno, "Could not open file to copy to", sourceFilePath);

        constexpr size BUFFER_SIZE = 4096;
        std::array<uint8, BUFFER_SIZE> buffer = { };

        size readByteCount = 0;
        while (readByteCount = read(sourceFileDescriptor, buffer.data(), buffer.size()), readByteCount > 0)
        {
            uint8* writePointer = buffer.data();

            size writtenByteCount = 0;
            do
            {
                writtenByteCount = write(destinationFileDescriptor, writePointer, readByteCount);
                if (writtenByteCount < 0)
                {
                    close(destinationFileDescriptor);
                    close(sourceFileDescriptor);

                    HandleUnixFileError(errno, "Could not copy from file", sourceFilePath);
                }

                readByteCount -= writtenByteCount;
                writePointer += writtenByteCount;

            } while (readByteCount > 0);
        }

        close(destinationFileDescriptor);
        close(sourceFileDescriptor);

        if (readByteCount != 0)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not copy file to [{0}]", resolvedFilePath.string()), sourceFilePath);
        }
    }

    void UnixFileManager::MoveFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::MoveFile(sourceFilePath, destinationDirectoryPath, conflictPolicy);

        CopyFile(sourceFilePath, destinationDirectoryPath, conflictPolicy);
        DeleteFile(sourceFilePath);
    }

    void UnixFileManager::DeleteFile(const std::filesystem::path& filePath) const
    {
        FileManager::DeleteFile(filePath);
        const std::string path = filePath.string();

        if (remove(path.c_str()) != 0)
        {
            HandleUnixFileError(errno, "Could not delete file", filePath);
        }
    }

    bool UnixFileManager::DirectoryExists(const std::filesystem::path& directoryPath) const noexcept
    {
        const std::string path = directoryPath.string();

        struct stat status = { };
        return (stat(path.c_str(), &status) == 0);
    }

    void UnixFileManager::EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, bool recursive, const FileEnumerationPredicate& Predicate) const
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

    void UnixFileManager::CreateDirectory(const std::filesystem::path& directoryPath) const
    {
        FileManager::CreateDirectory(directoryPath);
        if (DirectoryExists(directoryPath)) return;

        const std::string path = directoryPath.string();
        if (mkdir(path.c_str(), S_IRWXU) != 0)
        {
            HandleUnixFileError(errno, "Could not create directory", directoryPath);
        }
    }

    void UnixFileManager::RenameDirectory(const std::filesystem::path& directoryPath, const std::string_view name) const
    {
        FileManager::RenameDirectory(directoryPath, name);

        const std::string sourcePath = directoryPath.string();
        const std::string renamedPath = (directoryPath.parent_path() / name).string();

        if (rename(sourcePath.c_str(), renamedPath.c_str()) != 0)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not rename directory to [{0}]", name), directoryPath);
        }
    }

    void UnixFileManager::CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CopyDirectory(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedDirectoryPath = destinationDirectoryPath / sourceDirectoryPath.filename();
        ResolveDirectoryPathConflict(sourceDirectoryPath, resolvedDirectoryPath, conflictPolicy);
        CreateDirectory(resolvedDirectoryPath);

        std::error_code errorCode = { };
        std::filesystem::copy(sourceDirectoryPath, resolvedDirectoryPath, errorCode);

        if (errorCode.value() != 0)
        {
            HandleUnixFileError(errno, SR_FORMAT("Could not copy directory to [{0}]", resolvedDirectoryPath.string()), sourceDirectoryPath);
        }
    }

    void UnixFileManager::MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::MoveDirectory(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);

        CopyDirectory(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);
        DeleteDirectory(sourceDirectoryPath);
    }

    void UnixFileManager::DeleteDirectory(const std::filesystem::path& directoryPath) const
    {
        FileManager::DeleteDirectory(directoryPath);

        std::error_code errorCode = { };
        std::filesystem::remove_all(directoryPath, errorCode);

        if (errorCode.value() != 0)
        {
            HandleUnixFileError(errno, "Could not delete directory", directoryPath);
        }
    }

    /* --- GETTER METHODS --- */

    FileMetadata UnixFileManager::GetFileMetadata(const std::filesystem::path& filePath) const
    {
        const std::string path = filePath.string();

        struct stat status = { };
        if (stat(path.c_str(), &status) != 0) HandleUnixFileError(errno, "Could not get file metadata", filePath);

        const Date dateCreated = Date(TimePoint(std::chrono::system_clock::from_time_t(status.st_ctime)));
        const Date dateLastModified = Date(TimePoint(std::chrono::system_clock::from_time_t(status.st_mtime)));

        return
        {
            .type = FilePathToFileType(filePath),
            .memorySize = static_cast<size>(status.st_size),
            .dateCreated = dateCreated,
            .dateLastModified = dateLastModified
        };
    }

    DirectoryMetadata UnixFileManager::GetDirectoryMetadata(const std::filesystem::path& directoryPath) const
    {
        const std::string path = directoryPath.string();

        struct stat status = { };
        if (stat(path.c_str(), &status) != 0) HandleUnixFileError(errno, "Could not get directory metadata", directoryPath);

        const Date dateCreated = Date(TimePoint(std::chrono::system_clock::from_time_t(status.st_ctime)));
        const Date dateLastModified = Date(TimePoint(std::chrono::system_clock::from_time_t(status.st_mtime)));

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
    }

    std::filesystem::path UnixFileManager::GetApplicationDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath();
    }

    std::filesystem::path UnixFileManager::GetExecutableDirectoryPath() const noexcept
    {
        char buffer[PATH_MAX];
        realpath("/proc/self/exe", buffer);
        return buffer;
    }

    std::filesystem::path UnixFileManager::GetResourcesDirectoryPath() const noexcept
    {
        return GetExecutableDirectoryPath() / "Resources";
    }

    std::filesystem::path UnixFileManager::GetHomeDirectoryPath() const noexcept
    {
        return "/home";
    }

    std::filesystem::path UnixFileManager::GetUserDirectoryPath() const noexcept
    {
        return std::getenv("HOME");
    }

    std::filesystem::path UnixFileManager::GetTemporaryDirectoryPath() const noexcept
    {
        return std::filesystem::temp_directory_path();
    }

    std::filesystem::path UnixFileManager::GetCachesDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / ".cache";
    }

    std::filesystem::path UnixFileManager::GetDesktopDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Desktop";
    }

    std::filesystem::path UnixFileManager::GetDownloadsDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Downloads";
    }

    std::filesystem::path UnixFileManager::GetDocumentsDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Documents";
    }

    std::filesystem::path UnixFileManager::GetPicturesDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Pictures";
    }

    std::filesystem::path UnixFileManager::GetMusicDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Music";
    }

    std::filesystem::path UnixFileManager::GetVideosDirectoryPath() const noexcept
    {
        return GetUserDirectoryPath() / "Videos";
    }

}