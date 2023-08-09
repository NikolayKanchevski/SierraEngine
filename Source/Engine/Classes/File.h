//
// Created by Nikolay Kanchevski on 3.10.22.
//

#pragma once

/// @brief Provides useful methods for working with the file system.
namespace Sierra::Engine::File
{

    /* --- POLLING METHODS --- */
    void Initialize();

    /// @brief Directory path to the %APPDATA% folder.
    [[nodiscard]] FilePath GetAppDataFolderPath();

    /// @brief Directory path to the %TEMP% folder.
    [[nodiscard]] FilePath GetTempFolderPath();

    /// @brief Checks whether a specific file is present on the file system.
    [[nodiscard]] bool FileExists(const FilePath &filePath);

    /// @brief Checks whether a specific directory exists on the file system.
    [[nodiscard]] bool DirectoryExists(const FilePath &directoryPath);

    /// @brief Creates a new file on the file system.
    /// @param filePath Must follow this template: "SomeDirectory/SomeSubDirectory/SomeFileName.some_extension".
    bool CreateFile(const FilePath &filePath);

    /// @brief Reads data from a specified file and stores it as a vector of characters.
    [[nodiscard]] const std::vector<char> ReadFile(const FilePath &filePath);

    /// @brief Reads binary (8-bit) data from a specified file and stores it as a vector of uint8's.
    [[nodiscard]] const std::vector<uint8> ReadBinaryFile(const FilePath &filePath);

    /// @brief Writes a char array to a specified file.
    bool WriteDataToFile(const FilePath &filePath, const char* data, bool overwrite, bool createFile = false);

    /// @brief Writes a String to a specified file.
    bool WriteDataToFile(const FilePath &filePath, const String &data, bool overwrite, bool createFile = false);

    /// @brief Writes binary (8-bit) data to a spcified file.
    bool WriteBinaryDataToFile(const FilePath &filePath, const uint8* data, uint32 dataSize, bool overwrite, bool createFile = false);

    /// @brief Writes binary (8-bit) data to a spcified file.
    bool WriteBinaryDataToFile(const FilePath &filePath, const std::vector<uint8> &data, bool overwrite, bool createFile = false);

    /// @brief Removes a specific file from the file system.
    bool RemoveFile(const FilePath &filePath);

    /// @brief Creates a new file, regardless of whether there already is one with the same path (and overwrites it).
    bool OverwriteFile(const FilePath &filePath);

    /// @brief Creates a new directory (folder) on file the system.
    bool CreateDirectory(const FilePath &directoryPath);

    /// @brief Removes a given directory (folder) from the file system.
    bool RemoveDirectory(const FilePath &directoryPath);

    /// @brief Creates a new directory (folder) on file the system, regardless of whether there already is one with the same path (and overwrites it).
    bool OverwriteDirectory(const FilePath &directoryPath);

    /// @brief Creates all needed directories, in order to make a new path available.
    /// @param targetPath Must follow this template: "SomeDirectory/SomeSubdirectory/OtherSubdirectory/".
    bool CreateDirectoriesToPath(const FilePath &targetPath);

    /// @brief Returns a vector of strings, each representing a path of some of the subdirectories through which the specified path has gone.
    [[nodiscard]] std::vector<String> GetDirectoriesFromPath(const FilePath &path);

    /// @brief Searches for a given file tag within a directory and all of its subdirectories. Returns empty string if file not found.
    /// @param directory Which directory to search.</param>
    /// @param fileName Which file to search for. Must contain an extension!</param>
    [[nodiscard]] FilePath FindInSubdirectories(const FilePath &directory, const String &fileName);
}