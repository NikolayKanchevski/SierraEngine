//
// Created by Nikolay Kanchevski on 3.10.22.
//

#include "File.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace Sierra::Engine::Classes::File
{

    std::string TrimPath(const std::string &filePath)
    {
        size_t index = filePath.find_last_of('/') + 1;
        return filePath.substr(index);
    }

    std::string FindInSubdirectories(const std::string &directory, const std::string &fileName)
    {
        std::string fileExtension = fileName.substr(fileName.find_last_of('.') + 1);
        size_t index = directory.find_last_of('/') + 1;


        for (std::filesystem::recursive_directory_iterator i(directory), end; i != end; ++i)
        {
            if (!is_directory(i->path()) && i->path().filename() == fileName)
            {
                return i->path().relative_path().string().substr(index);
            }
        }

        return "";
    }

    const std::vector<char> ReadFile(const std::string &fileName)
    {
        // Open stream to read from given file (as binary, at the end)
        std::ifstream file(fileName, std::ios::binary | std::ios::ate);

        // Check if the file stream successfully opened
        if (!file.is_open())
        {
            std::runtime_error("Failed to open file!");
        }

        // Get file size and create resized vector
        size_t fileSize = static_cast<size_t>(file.tellg());
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