//
// Created by Nikolay Kanchevski on 3.10.22.
//

#pragma once

#include <string>

/// @brief Provides useful methods for working with the file system.
namespace Sierra::Engine::Classes::File
{
    /// @brief Returns only the last directory/file from a given path.
    /// @param filePath Which file path to use.
    [[nodiscard]] std::string GetFileNameFromPath(const std::string &filePath, bool includeExtension = true);

    /// @brief Returns a given file path without the file tag and extension at the end.
    /// @param filePath What file path to use.
    [[nodiscard]] std::string RemoveFileNameFromPath(const std::string &filePath);

    /// @brief Gets the file extension of a file in a given path.
    /// @param filePath Path to the file on system.
    /// @param includeDot Whether to add the '.' symbol at the beginning of the returned value. False by default.
    [[nodiscard]] std::string GetFileExtensionFromPath(const std::string &filePath, bool includeDot = false);

    /// @brief Searches for a given file tag within a directory and all of its subdirectories. Returns empty string if file not found.
    /// @param directory Which directory to search.</param>
    /// @param fileName Which file to search for. Must contain an extension!</param>
    [[nodiscard]] std::string FindInSubdirectories(const std::string &directory, const std::string &fileName);

    /// @brief Reads a given file and returns its data as a vector of characters.
    /// @param fileName Location of file to read.
    [[nodiscard]] const std::vector<char> ReadFile(const std::string &fileName);

}