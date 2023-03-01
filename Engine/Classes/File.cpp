//
// Created by Nikolay Kanchevski on 3.10.22.
//

#include "File.h"

namespace Sierra::Engine::Classes
{
    using Core::Debugger;

    const String File::OUTPUT_FOLDER_PATH = "NULL";

    bool File::FileExists(const String &filePath)
    {
        struct stat buffer;
        return stat(filePath.c_str(), &buffer) == 0;
    }

    bool File::FileExists(const String &directory, const String &fileName)
    {
        return FileExists(directory + fileName);
    }

    bool File::DirectoryExists(const String &directoryPath)
    {
        struct stat buffer;
        return stat(directoryPath.c_str(), &buffer) == 0;
    }

    bool File::DirectoryExists(const String &where, const String &directoryName)
    {
        return DirectoryExists(where + directoryName);
    }

    bool File::CreateFile(const String &filePath)
    {
        if (FileExists(filePath))
        {
            return true;
        }

        bool success = true;
        success = success && CreateDirectoriesToPath(filePath);

        std::ofstream file(filePath);

        if (!file.is_open() || file.fail())
        {
            ASSERT_WARNING_FORMATTED("Could not create file with a path of [{0}]", filePath);
            success = false;
        }
        else
        {
            file.close();
        }

        return success;
    }

    bool File::CreateFile(const String &directory, const String &fileName)
    {
        return CreateFile(directory + fileName);
    }

    bool File::WriteDataToFile(const String &filePath, const char *data, const bool createFile)
    {
        if (createFile)
        {
            CreateFile(filePath);
        }

        if (!FileExists(filePath))
        {
            ASSERT_WARNING_FORMATTED("Trying to write data to a file with a path of [{0}] but the file does not exist", filePath);
            return false;
        }

        std::ofstream file(filePath, std::ios::out | std::ios::binary | std::ios::trunc);

        file << data;

        file.close();

        return true;
    }

    bool File::WriteDataToFile(const String &directory, const String &fileName, const char* data, const bool createFile)
    {
        return WriteDataToFile(directory + fileName, data, createFile);
    }

    bool File::WriteDataToFile(const String &filePath, const String &data, const bool createFile)
    {
        return WriteDataToFile(filePath, data.c_str(), createFile);
    }

    bool File::WriteDataToFile(const String &directory, const String &fileName, const String &data, const bool createFile)
    {
        return WriteDataToFile(directory + fileName, data.c_str(), createFile);
    }

    bool File::WriteBinaryDataToFile(const String &filePath, const uint8 *data, const uint dataSize, const bool createFile)
    {
        if (createFile)
        {
            CreateFile(filePath);
        }

        if (!FileExists(filePath))
        {
            ASSERT_WARNING_FORMATTED("Trying to write data to a file with a path of [{0}] but the file does not exist", filePath);
            return false;
        }

        std::ofstream file(filePath, std::ios::out | std::ios::binary | std::ios::trunc);

        ASSERT_ERROR_FORMATTED_IF(!file.is_open() || file.fail(), "Could not open the file [{0}] for writing to it", filePath);

        file.write(reinterpret_cast<const char *>(data), dataSize);
        file.close();

        return true;
    }

    bool File::WriteBinaryDataToFile(const String &directory, const String &fileName, const uint8 *data, const uint dataSize, const bool createFile)
    {
        return WriteBinaryDataToFile(directory + fileName, data, dataSize, createFile);
    }

    bool File::WriteBinaryDataToFile(const String &filePath, const std::vector<uint8> &data, const bool createFile)
    {
        return WriteBinaryDataToFile(filePath, data.data(), data.size(), createFile);
    }

    bool File::WriteBinaryDataToFile(const String &directory, const String &fileName, const std::vector<uint8> &data, const bool createFile)
    {
        return WriteBinaryDataToFile(directory + fileName, data.data(), data.size(), createFile);
    }

    const std::vector<char> File::ReadFile(const String &filePath)
    {
        ASSERT_ERROR_FORMATTED_IF(!FileExists(filePath), "Trying to read data from a file with a path of [{0}] but the file does not exist", filePath);

        // Open stream to read from given file (as binary, at the end)
        std::ifstream file(filePath, std::ios::binary | std::ios::ate);

        // Check if the file stream successfully opened
        ASSERT_ERROR_FORMATTED_IF(!file.is_open() || file.fail(), "Could not open the file [{0}] for reading from it", filePath);

        // Get file size and create resized vector
        uSize fileSize = static_cast<uSize>(file.tellg());
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

    const std::vector<char> File::ReadFile(const String &directory, const String &fileName)
    {
        return ReadFile(directory + fileName);
    }

    const std::vector<uint8> File::ReadBinaryFile(const String &filePath)
    {
        ASSERT_ERROR_FORMATTED_IF(!FileExists(filePath), "Trying to read data from a file with a path of [{0}] but the file does not exist", filePath);

        std::ifstream file(filePath, std::ios::binary | std::ios::in | std::ios::ate);

        ASSERT_ERROR_FORMATTED_IF(!file.is_open() || file.fail(), "Could not open the file [{0}] for reading from it", filePath);

        uSize fileSize = static_cast<uSize>(file.tellg());
        std::vector<uint8> data(fileSize);

        file.seekg(0);
        file.read(reinterpret_cast<char *>(data.data()), fileSize);
        file.close();

        return data;
    }

    const std::vector<uint8> File::ReadBinaryFile(const String &directory, const String &fileName)
    {
        return ReadBinaryFile(directory + fileName);
    }

    bool File::RemoveFile(const String &filePath)
    {
        return remove(filePath.c_str()) == 0;
    }

    bool File::RemoveFile(const String &directory, const String &fileName)
    {
        return RemoveFile(directory + fileName);
    }

    bool File::OverwriteFile(const String &filePath)
    {
        bool success = true;
        if (FileExists(filePath))
        {
            success = success && RemoveFile(filePath);
        }

        success = success && CreateFile(filePath);

        return success;
    }

    bool File::OverwriteFile(const String &directory, const String &fileName)
    {
        return OverwriteFile(directory + fileName);
    }

    bool File::CreateDirectory(const String &directoryPath)
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

    bool File::CreateDirectory(const String &where, const String &directoryName)
    {
        return CreateDirectory(where + directoryName);
    }

    bool File::RemoveDirectory(const String &directoryPath)
    {
        if (!DirectoryExists(directoryPath))
        {
            return false;
        }

        return rmdir(directoryPath.c_str()) == 0;
    }

    bool File::RemoveDirectory(const String &where, const String &directoryName)
    {
        return RemoveDirectory(where + directoryName);
    }

    bool File::OverwriteDirectory(const String &directoryPath)
    {
        bool success = true;
        if (DirectoryExists(directoryPath))
        {
            success = success && RemoveDirectory(directoryPath);
        }

        success = success && CreateDirectory(directoryPath);

        return success;
    }

    bool File::OverwriteDirectory(const String &where, const String &directoryName)
    {
        return OverwriteDirectory(where + directoryName);
    }

    bool File::CreateDirectoriesToPath(const String &targetPath)
    {
        auto directories = GetDirectoriesFromPath(targetPath);

        bool success = true;
        for (const auto &directory : directories)
        {
            success = success && CreateDirectory(directory);
        }

        return success;
    }

    String File::GetFileNameFromPath(const String &filePath, const bool includeExtension)
    {
        uSize startIndex = filePath.find_last_of('/') + 1;
        uSize endIndex = filePath.size() - startIndex;

        if (!includeExtension)
        {
            endIndex -= GetFileExtensionFromPath(filePath, true).size();
        }

        return filePath.substr(startIndex, endIndex);
    }

    String File::RemoveFileNameFromPath(const String &filePath)
    {
        uSize index = filePath.find_last_of('/') + 1;
        return filePath.substr(0, index);
    }

    String File::GetFileExtensionFromPath(const String &filePath, const bool includeDot)
    {
        uSize index = filePath.find_last_of('.') + !includeDot;
        return filePath.substr(index);
    }

    std::vector<String> File::GetDirectoriesFromPath(const String &path)
    {
        std::vector<String> directories;

        uSize index = 0;
        while (index < path.size())
        {
            if (path[index] == '/')
            {
                directories.push_back(path.substr(0, index) + '/');
            }

            index++;
        }

        return directories;
    }

    String File::FindInSubdirectories(const String &directory, const String &fileName)
    {
        for (std::filesystem::recursive_directory_iterator i(directory), end; i != end; ++i)
        {
            if (!is_directory(i->path()) && i->path().filename() == fileName)
            {
                return i->path().string().substr(directory.size());
            }
        }

        return "";
    }

    void File::Start()
    {
        // Get output directory
        int length, dirnameLength;
        length = wai_getExecutablePath(nullptr, 0, &dirnameLength);

        if (length > 0)
        {
            char* path = new char[length + 1];
            wai_getExecutablePath(path, length, &dirnameLength);

            path[dirnameLength] = '\0';

            String pathString = String(path) + '/';
            std::replace(pathString.begin(), pathString.end(), '\\', '/');
            MODIFY_CONST(String, OUTPUT_FOLDER_PATH, pathString);

            free(path);
        }
        else
        {
            ASSERT_WARNING("Could not retrieve output directory");
        }
    }
}