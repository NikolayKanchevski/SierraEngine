//
// Created by Nikolay Kanchevski on 3.10.22.
//

#include "File.h"

namespace Sierra::Engine::Classes::File
{
    using Core::Debugger;

    String GetFileNameFromPath(const String &filePath, const bool includeExtension)
    {
        uSize startIndex = filePath.find_last_of('/') + 1;
        uSize endIndex = filePath.size() - startIndex;

        if (!includeExtension)
        {
            endIndex -= GetFileExtensionFromPath(filePath, true).size();
        }

        return filePath.substr(startIndex, endIndex);
    }

    String RemoveFileNameFromPath(const String &filePath)
    {
        uSize index = filePath.find_last_of('/') + 1;
        return filePath.substr(0, index);
    }

    String GetFileExtensionFromPath(const String &filePath, const bool includeDot)
    {
        uSize index = filePath.find_last_of('.') + !includeDot;
        return filePath.substr(index);
    }

    String FindInSubdirectories(const String &directory, const String &fileName)
    {
        String fileExtension = fileName.substr(fileName.find_last_of('.') + 1);
        uSize index = directory.find_last_of('/') + 1;

        for (std::filesystem::recursive_directory_iterator i(directory), end; i != end; ++i)
        {
            if (!is_directory(i->path()) && i->path().filename() == fileName)
            {
                return i->path().relative_path().string().substr(index);
            }
        }

        return "";
    }

    const std::vector<char> ReadFile(const String &fileName)
    {
        // Open stream to read from given file (as binary, at the end)
        std::ifstream file(fileName, std::ios::binary | std::ios::ate);

        // Check if the file stream successfully opened
        ASSERT_ERROR_IF(file.tellg() <= 0 || !file.is_open() || file.fail(), "Could not open the file [" + fileName + "]");

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

}