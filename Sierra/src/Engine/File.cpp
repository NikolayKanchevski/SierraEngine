//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "File.h"

#if SR_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>

    #undef CreateFile
    #undef CreateDirectory
    #undef DeleteFile
    #undef DeleteDirectory
#elif SR_PLATFORM_APPLE
    #include "Platform/Apple/NSFilePaths.h"
#endif

namespace Sierra
{
    
    /* --- POLLING METHODS --- */

    bool File::CreateFile(const std::filesystem::path &filePath)
    {
        if (FileExists(filePath)) return true;
        if (!CreateDirectory(filePath.parent_path())) return false;

        // Try to open file (stream creates a file if missing)
        std::ofstream file(filePath);
        if (!file.is_open()) return false;
        file.close();

        return true;
    }

    bool File::CreateDirectory(const std::filesystem::path &directoryPath)
    {
        if (DirectoryExists(directoryPath)) return true;
        return std::filesystem::create_directories(directoryPath);
    }

    bool File::DeleteFile(const std::filesystem::path &filePath)
    {
        if (!FileExists(filePath)) return true;
        return remove(filePath.string().c_str()) == 0;
    }

    bool File::DeleteDirectory(const std::filesystem::path &directoryPath)
    {
        if (!DirectoryExists(directoryPath)) return true;
        return std::filesystem::remove(directoryPath);
    }

    std::vector<char> File::ReadFile(const std::filesystem::path &filePath)
    {
        SR_ERROR_IF(!FileExists(filePath), "Could not open file [{0}] for reading, as it does not exist!", filePath.string().c_str());

        // Try to create a stream to read file
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        SR_ERROR_IF(!file.is_open(), "Could not open file [{0}] for reading!", filePath.string().c_str());

        // Get file size and create resized vector
        const std::streamsize fileSize = static_cast<int64>(file.tellg());
        std::vector<char> fileBuffer(fileSize);

        // Sava file data to buffer
        file.seekg(0);
        file.read(fileBuffer.data(), fileSize);

        // Close file and return data
        file.close();
        return fileBuffer;
    }

    bool File::WriteToFile(const std::filesystem::path &filePath, const char* data, const uint64 dataSize, const bool overwrite, const bool createFile)
    {
        if (createFile) CreateFile(filePath);
        if (!FileExists(filePath)) return false;

        // Write data to file
        std::ofstream file(filePath, std::ios::out | std::ios::binary | (overwrite ? std::ios::trunc : std::ios::app));
        if (!file.is_open()) return false;

        // Write to file and close
        file.write(data, static_cast<std::streamsize>(dataSize));
        file.close();

        return true;
    }

    /* --- GETTER METHODS --- */
    
    bool File::FileExists(const std::filesystem::path &filePath)
    {
        return std::filesystem::exists(filePath);
    }

    bool File::DirectoryExists(const std::filesystem::path &directoryPath)
    {
        return std::filesystem::exists(directoryPath);
    }

    const std::filesystem::path& File::GetResourcesDirectoryPath()
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
                resourcesDirectoryPath = NSFilePaths::GetApplicationDirectoryPath();
            #endif
        }
        return resourcesDirectoryPath;
    }

    const std::filesystem::path& File::GetCachesDirectoryPath()
    {
        static const std::filesystem::path cachesDirectoryPath =
            #if SR_PLATFORM_WINDOWS || SR_PLATFORM_LINUX || SR_PLATFORM_ANDROID
                GetTemporaryDirectoryPath();
            #elif SR_PLATFORM_APPLE
                NSFilePaths::GetCachesDirectoryPath();
            #endif
        return cachesDirectoryPath;
    }

    const std::filesystem::path& File::GetTemporaryDirectoryPath()
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