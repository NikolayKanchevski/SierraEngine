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

    std::vector<uint8> File::ReadFile(const std::filesystem::path &filePath, const uint64 sourceByteOffset, uint64 memoryRange)
    {
        SR_ERROR_IF(!FileExists(filePath), "Could not open file [{0}] for reading, as it does not exist!", filePath.string().c_str());

        // Try to create a stream to read file
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);
        SR_ERROR_IF(!file.is_open(), "Could not open file [{0}] for reading!", filePath.string());

        // Get file size and create resized vector
        memoryRange = memoryRange == 0 ? static_cast<uint64>(file.tellg()) : memoryRange;
        std::vector<uint8> fileBuffer(memoryRange);

        // Sava file data to buffer
        file.seekg(sourceByteOffset);
        file.read(reinterpret_cast<char*>(fileBuffer.data()), memoryRange);

        // Close file and return data
        file.close();
        return fileBuffer;
    }

    bool File::WriteToFile(const std::filesystem::path &filePath, const void* memory, const uint64 memoryRange, const uint64 sourceByteOffset, const uint64 destinationByteOffset, const FileWriteFlags writeFlags)
    {
        if (writeFlags & FileWriteFlags::Create) CreateFile(filePath);
        if (!FileExists(filePath)) return false;

        // Write data to file
        std::ofstream file(filePath, std::ios::out | std::ios::binary | ((writeFlags & FileWriteFlags::Overwrite) ? std::ios::trunc : std::ios::app));
        if (!file.is_open()) return false;

        // Write to file and close
        file.seekp(destinationByteOffset);
        file.write(reinterpret_cast<const char*>(memory) + sourceByteOffset, static_cast<std::streamsize>(memoryRange));
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
                #if SR_PLATFORM_macOS
                    resourcesDirectoryPath = NSFilePaths::GetApplicationDirectoryPath() / "Contents" / "Resources";
                #elif SR_PLATFORM_iOS
                    resourcesDirectoryPath = NSFilePaths::GetApplicationDirectoryPath();
                #endif
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