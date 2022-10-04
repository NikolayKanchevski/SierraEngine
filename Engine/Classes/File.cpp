//
// Created by Nikolay Kanchevski on 3.10.22.
//

#include "File.h"

#include <filesystem>
#include <iostream>

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

}