//
// Created by Nikolay Kanchevski on 3.10.22.
//

#include "File.h"

namespace Sierra::Engine
{
    /* --- PROPERTIES --- */

    FilePath appDataDirectory = "/";
    FilePath tempFolderPath = "/";

    /* --- POLLING  METHODS --- */

    void File::Initialize()
    {
        // Get app data folder
        appDataDirectory = sago::getDataHome() + '/';

        // Get temp folder
        String tempFolder = std::filesystem::temp_directory_path().string();
        if (tempFolder.back() != '/') tempFolder += '/';
        tempFolderPath = tempFolder;
    }

    /* --- GETTER METHODS --- */

    FilePath File::GetAppDataFolderPath()
    {
        return appDataDirectory;
    }

    FilePath File::GetTempFolderPath()
    {
        return tempFolderPath;
    }

    bool File::FileExists(const FilePath &filePath)
    {
        struct stat buffer;
        return stat(filePath.c_str(), &buffer) == 0;
    }

    bool File::DirectoryExists(const FilePath &directoryPath)
    {
        struct stat buffer;
        return stat(directoryPath.c_str(), &buffer) == 0;
    }

    bool File::CreateFile(const FilePath &filePath)
    {
        if (FileExists(filePath))
        {
            return true;
        }

        bool success = CreateDirectoriesToPath(filePath);

        std::ofstream file(filePath);

        if (!file.is_open() || file.fail())
        {
            ASSERT_WARNING("Could not create file with a path of [{0}]", filePath.c_str());
            success = false;
        }
        else
        {
            file.close();
        }

        return success;
    }

    bool File::WriteDataToFile(const FilePath &filePath, const char* data, const bool overwrite, const bool createFile)
    {
        if (createFile)
        {
            CreateFile(filePath);
        }

        if (!FileExists(filePath))
        {
            ASSERT_WARNING("Trying to write data to a file with a path of [{0}] but the file does not exist", filePath.c_str());
            return false;
        }

        std::ofstream file(filePath, std::ios::out | std::ios::binary | (overwrite ? std::ios::trunc : std::ios::app));

        file << data;

        file.close();

        return true;
    }

    bool File::WriteBinaryDataToFile(const FilePath &filePath, const uint8* data, const uint32 dataSize, const bool overwrite, const bool createFile)
    {
        if (createFile)
        {
            CreateFile(filePath);
        }

        if (!FileExists(filePath))
        {
            ASSERT_WARNING("Trying to write data to a file with a path of [{0}] but the file does not exist", filePath.c_str());
            return false;
        }

        std::ofstream file(filePath, std::ios::out | std::ios::binary | (overwrite ? std::ios::trunc : std::ios::app));

        ASSERT_ERROR_IF(!file.is_open() || file.fail(), "Could not open the file [{0}] for writing to it", filePath.c_str());

        file.write(reinterpret_cast<const char*>(data), dataSize);
        file.close();

        return true;
    }

    bool File::WriteBinaryDataToFile(const FilePath &filePath, const std::vector<uint8> &data, const bool overwrite, const bool createFile)
    {
        return WriteBinaryDataToFile(filePath, data.data(), data.size() * UINT8_SIZE, overwrite, createFile);
    }

    const std::vector<char> File::ReadFile(const FilePath &filePath)
    {
        ASSERT_ERROR_IF(!FileExists(filePath), "Trying to read data from a file with a path of [{0}] but the file does not exist", filePath.c_str());

        // Open stream to read from given file (as binary, at the end)
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);

        // Check if the file stream successfully opened
        ASSERT_ERROR_IF(!file.is_open() || file.fail(), "Could not open the file [{0}] for reading from it", filePath.c_str());

        // Get file size and create resized vector
        std::streamsize fileSize = static_cast<uSize>(file.tellg());
        std::vector<char> fileBuffer(fileSize);

        // Go back to the start of file
        file.seekg(0);

        // Read file and save it to the file buffer
        file.read(fileBuffer.data(), fileSize);

        // Close file stream
        file.close();

        // Return read file
        return fileBuffer;
    }

    const std::vector<uint8> File::ReadBinaryFile(const FilePath &filePath)
    {
        ASSERT_ERROR_IF(!FileExists(filePath), "Trying to read data from a file with a path of [{0}] but the file does not exist", filePath.c_str());

        std::ifstream file(filePath, std::ios::binary | std::ios::in | std::ios::ate);

        ASSERT_ERROR_IF(!file.is_open() || file.fail(), "Could not open the file [{0}] for reading from it", filePath.c_str());

        uSize fileSize = static_cast<uSize>(file.tellg());
        std::vector<uint8> data(fileSize);

        file.seekg(0);
        file.read(reinterpret_cast<char* >(data.data()), fileSize);
        file.close();

        return data;
    }

    bool File::RemoveFile(const FilePath &filePath)
    {
        return remove(filePath.c_str()) == 0;
    }

    bool File::OverwriteFile(const FilePath &filePath)
    {
        bool success = true;
        if (FileExists(filePath))
        {
            success = RemoveFile(filePath);
        }

        success = success && CreateFile(filePath);

        return success;
    }

    bool File::CreateDirectory(const FilePath &directoryPath)
    {
        if (DirectoryExists(directoryPath))
        {
            return true;
        }

        #if defined _MSC_VER
            return _mkdir(directoryPath.c_str()) == 0;
        #elif defined __GNUC__
            return mkdir(directoryPath.c_str(), 0777) == 0;
        #endif
    }

    bool File::RemoveDirectory(const FilePath &directoryPath)
    {
        if (!DirectoryExists(directoryPath))
        {
            return false;
        }

        return rmdir(directoryPath.c_str()) == 0;
    }

    bool File::OverwriteDirectory(const FilePath &directoryPath)
    {
        bool success = true;
        if (DirectoryExists(directoryPath))
        {
            success = RemoveDirectory(directoryPath);
        }

        success = success && CreateDirectory(directoryPath);

        return success;
    }

    bool File::CreateDirectoriesToPath(const FilePath &targetPath)
    {
        auto directories = GetDirectoriesFromPath(targetPath);

        bool success = true;
        for (const auto &directory : directories)
        {
            success = success && CreateDirectory(directory);
        }

        return success;
    }

    std::vector<String> File::GetDirectoriesFromPath(const FilePath &path)
    {
        std::vector<String> paths;
        for (const auto &directory : path)
        {
            paths.push_back(relative(directory, directory.parent_path()).filename().string());
        }

        return paths;
    }

    FilePath File::FindInSubdirectories(const FilePath &directory, const String &fileName)
    {
        for (const auto &file : std::filesystem::recursive_directory_iterator(directory))
        {
            if (!is_directory(file.path()) && file.path().filename() == fileName)
            {
                return relative(file.path(), directory);
            }
        }

        return "";
    }
}