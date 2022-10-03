//
// Created by Nikolay Kanchevski on 3.10.22.
//

#pragma once

#include <string>

namespace Sierra::Engine::Classes
{

    class File
    {
    public:
        /// \brief Returns only the last directory/file from a given path.
        /// @param filePath Which file path to use.
        static std::string TrimPath(const std::string &filePath);

        /// \brief Searches for a given file name within a directory and all of its subdirectories. Returns empty string if file not found.
        /// @param directory Which directory to search.</param>
        /// @param fileName Which file to search for. Must contain an extension!</param>
        static std::string FindInSubdirectories(const std::string &directory, const std::string &fileName);
    private:

    };

}