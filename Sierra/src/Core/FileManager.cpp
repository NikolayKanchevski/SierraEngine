//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "FileManager.h"

#if SR_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    #undef CreateFile
    #undef CopyFile
    #undef MoveFile
    #undef CreateDirectory
    #undef DeleteFile
    #undef DeleteDirectory
#elif SR_PLATFORM_APPLE
    #include "../Utilities/Platform/Apple/NSFilePaths.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */
    
    File::File(const std::filesystem::path &path, std::fstream&& stream)
        : path(path), stream(std::move(stream))
    {
        SR_ERROR_IF(!this->stream.is_open() || this->stream.fail(), "Could not open file [{0}], as provided file stream is invalid!", path.string());
    }

    /* --- POLLING METHODS --- */

    void File::Seek(const uint64 index)
    {
        stream.seekg(static_cast<std::streamoff>(index));
    }

    void File::Resize(const uint64 memorySize)
    {
        std::filesystem::resize_file(path, memorySize);
    }

    std::vector<uint8> File::Read(uint64 length)
    {
        length = length != 0 ? length : GetSize();
        SR_ERROR_IF(static_cast<uint64>(stream.tellg()) + length > GetSize(), "Cannot read [{0}] bytes, offset by another [{1}], from file, as the memory range exceeds the file's size [{2}]!", length, static_cast<uint64>(stream.tellg()), GetSize());

        std::vector<uint8> buffer(length);
        stream.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamoff>(length));

        return buffer;
    }

    void File::Write(const void* data, const uint64 memorySize)
    {
        stream.write(reinterpret_cast<const char*>(data), static_cast<std::streamoff>(memorySize));
    }

    /* --- CONSTRUCTORS --- */

    FileManager::FileManager(const FileManagerCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    std::optional<Sierra::File> FileManager::OpenFile(const std::filesystem::path &filePath, const FileAccess access) const
    {
        std::fstream stream(filePath, std::ios::binary | (access == FileAccess::ReadOnly ? std::ios::in : 0) | (access == FileAccess::WriteOnly ? std::ios::out : 0) | (access == FileAccess::ReadWrite ? std::ios::in | std::ios::out : 0));
        if (!stream.is_open() || stream.fail()) return std::nullopt;
        return File(filePath, std::move(stream));
    }

    std::optional<File> FileManager::CreateAndOpenFile(const std::filesystem::path &filePath, const FileAccess access) const
    {
        std::fstream stream(filePath, std::ios::binary | std::ios::out | (access == FileAccess::ReadOnly ? std::ios::in : 0));
        if (!stream.is_open() || stream.fail()) return std::nullopt;
        return File(filePath, std::move(stream));
    }

    bool FileManager::CreateFile(const std::filesystem::path &filePath) const
    {
        const std::ofstream stream(filePath);
        return stream.is_open() && !stream.fail();
    }

    bool FileManager::FileExists(const std::filesystem::path &path) const
    {
        return std::filesystem::is_regular_file(path) && std::filesystem::exists(path);
    }

    bool FileManager::CopyFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path &destinationDirectory, const bool overwrite) const
    {
        if (!FileExists(sourceFilePath)) return false;
        if (!DirectoryExists(destinationDirectory) && !CreateDirectory(destinationDirectory)) return false;

        std::error_code error;
        std::filesystem::copy_file(sourceFilePath, destinationDirectory / sourceFilePath.filename(), overwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none, error);
        return static_cast<bool>(error);
    }

    bool FileManager::MoveFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path &destinationDirectory, const bool overwrite) const
    {
        if (!CopyFile(sourceFilePath, destinationDirectory, overwrite)) return false;
        if (!DeleteFile(sourceFilePath)) return false;
        return true;
    }

    bool FileManager::DeleteFile(const std::filesystem::path &filePath) const
    {
        return std::filesystem::remove(filePath);
    }

    bool FileManager::DirectoryExists(const std::filesystem::path &directoryPath) const
    {
        return std::filesystem::is_directory(directoryPath) && std::filesystem::exists(directoryPath);
    }

    bool FileManager::CreateDirectory(const std::filesystem::path &directoryPath) const
    {
        return std::filesystem::create_directories(directoryPath);
    }

    bool FileManager::CopyDirectory(const std::filesystem::path &sourceDirectory, const std::filesystem::path &destinationDirectory, const bool overwrite) const
    {
        if (!DirectoryExists(sourceDirectory)) return false;
        if (!DirectoryExists(destinationDirectory) && !CreateDirectory(destinationDirectory)) return false;

        std::error_code error;
        std::filesystem::copy(sourceDirectory, destinationDirectory, std::filesystem::copy_options::recursive | (overwrite ? std::filesystem::copy_options::overwrite_existing : std::filesystem::copy_options::none), error);
        return static_cast<bool>(error);
    }

    bool FileManager::MoveDirectory(const std::filesystem::path &sourceDirectory, const std::filesystem::path &destinationDirectory, const bool overwrite) const
    {
        if (!CopyFile(sourceDirectory, destinationDirectory, overwrite)) return false;
        if (!DeleteFile(sourceDirectory)) return false;
        return true;
    }

    bool FileManager::DeleteDirectory(const std::filesystem::path &directoryPath) const
    {
        if (!DirectoryExists(directoryPath)) return true;
        return std::filesystem::remove(directoryPath);
    }

    const std::filesystem::path &FileManager::GetResourcesDirectoryPath() const
    {
        static std::filesystem::path resourcesDirectoryPath;
        if (resourcesDirectoryPath.empty())
        {
            #if SR_PLATFORM_WINDOWS
                CHAR path[MAX_PATH];
                GetModuleFileNameA(nullptr, path, MAX_PATH);
                resourcesDirectoryPath = path;
                resourcesDirectoryPath = resourcesDirectoryPath.parent_path() / "Resources";
            #elif SR_PLATFORM_LINUX || SR_PLATFORM_ANDROID
                char path[4096];
                SR_ERROR_IF( readlink("/proc/self/exe", path, 4096) == -1, "Could not extract Linux binary folder!");
                resourcesDirectoryPath = path;
                resourcesDirectoryPath = resourcesDirectoryPath.parent_path() / "Resources";
            #elif SR_PLATFORM_APPLE
                #if SR_PLATFORM_macOS
                    resourcesDirectoryPath = NSFilePaths::GetApplicationDirectoryPath() / "Contents" / "Resources";
                #elif SR_PLATFORM_iOS
                    resourcesDirectoryPath = NSFilePaths::GetApplicationDirectoryPath();
                #endif
            #endif
        }
        return resourcesDirectoryPath;
    }

    const std::filesystem::path& FileManager::GetCachesDirectoryPath() const
    {
        static const std::filesystem::path cachesDirectoryPath =
            #if SR_PLATFORM_WINDOWS || SR_PLATFORM_LINUX || SR_PLATFORM_ANDROID
                GetTemporaryDirectoryPath();
            #elif SR_PLATFORM_APPLE
                NSFilePaths::GetCachesDirectoryPath();
            #endif
        return cachesDirectoryPath;
    }

    const std::filesystem::path& FileManager::GetTemporaryDirectoryPath() const
    {
        static const std::filesystem::path temporaryDirectoryPath =
            #if SR_PLATFORM_WINDOWS || SR_PLATFORM_LINUX || SR_PLATFORM_ANDROID
                std::filesystem::temp_directory_path();
            #elif SR_PLATFORM_APPLE
                NSFilePaths::GetTemporaryDirectoryPath();
            #endif
        return temporaryDirectoryPath;
    }

}