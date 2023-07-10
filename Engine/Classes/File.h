//
// Created by Nikolay Kanchevski on 3.10.22.
//

#pragma once

/// @brief Provides useful methods for working with the file system.
namespace Sierra::Engine::File
{
    /// @brief Directory path to the %TEMP% folder.
    const extern String TEMP_FOLDER_PATH;

    /// @brief Directory path to the folder, where internal engine data is stored (mainly cache data).
    const extern String INTERNAL_TEMP_FOLDER_PATH;

    /// @brief Path to the directory where the executable launched (the program) is located.
    const extern String OUTPUT_FOLDER_PATH;

    /// @brief Checks whether a specific file is present on the file system.
    [[nodiscard]] bool FileExists(const String &filePath);

    /// @brief Checks whether a specific file is present on the file system.
    [[nodiscard]] bool FileExists(const String &directory, const String &fileName);

    /// @brief Checks whether a specific directory exists on the file system.
    [[nodiscard]] bool DirectoryExists(const String &directoryPath);

    /// @brief Checks whether a specific directory exists on the file system.
    [[nodiscard]] bool DirectoryExists(const String &where, const String &directoryName);

    /// @brief Creates a new file on the file system.
    /// @param filePath Must follow this template: "SomeDirectory/SomeSubDirectory/SomeFileName.some_extension".
    bool CreateFile(const String &filePath);

    /// @brief Creates a new file on the file system.
    /// @param directory Must follow this template: "SomeDirectory/SomeSubDirectory/".
    /// @param fileName Must follow this template: "SomeFileName.some_extension/".
    bool CreateFile(const String &directory, const String &fileName);

    /// @brief Reads data from a spcified file and stores it as a vector of characters.
    [[nodiscard]] const std::vector<char> ReadFile(const String &filePath);

    /// @brief Reads data from a spcified file and stores it as a vector of characters.    
    [[nodiscard]] const std::vector<char> ReadFile(const String &directory, const String &fileName);

    /// @brief Reads binary (8-bit) data from a specified file and stores it as a vector of uint8's.
    [[nodiscard]] const std::vector<uint8> ReadBinaryFile(const String &filePath);

    /// @brief Reads binary (8-bit) data from a specified file and stores it as a vector of uint8's.
    [[nodiscard]] const std::vector<uint8> ReadBinaryFile(const String &directory, const String &fileName);

    /// @brief Writes a char array to a specified file.
    bool WriteDataToFile(const String &filePath, const char* data, bool createFile = false);

    /// @brief Writes a char array to a specified file.
    bool WriteDataToFile(const String &directory, const String &fileName, const char* data, bool createFile = false);

    /// @brief Writes a String to a specified file.
    bool WriteDataToFile(const String &filePath, const String &data, bool createFile = false);

    /// @brief Writes a String to a specified file.
    bool WriteDataToFile(const String &directory, const String &fileName, const String &data, bool createFile = false);

    /// @brief Writes binary (8-bit) data to a spcified file.
    bool WriteBinaryDataToFile(const String &filePath, const uint8 *data, uint dataSize, bool createFile = false);

    /// @brief Writes binary (8-bit) data to a spcified file.
    bool WriteBinaryDataToFile(const String &directory, const String &fileName, const uint8 *data, uint dataSize, bool createFile = false);

    /// @brief Writes binary (8-bit) data to a spcified file.
    bool WriteBinaryDataToFile(const String &filePath, const std::vector<uint8> &data, bool createFile = false);

    /// @brief Writes binary (8-bit) data to a spcified file.
    bool WriteBinaryDataToFile(const String &directory, const String &fileName, const std::vector<uint8> &data, bool createFile = false);

    /// @brief Removes a specific file from the file system.
    bool RemoveFile(const String &filePath);

    /// @brief Removes a specific file from the file system.
    bool RemoveFile(const String &directory, const String &fileName);

    /// @brief Creates a new file, regardless of whether there already is one with the same path (and overwrites it).
    bool OverwriteFile(const String &filePath);

    /// @brief Creates a new file, regardless of whether there already is one with the same path (and overwrites it).
    bool OverwriteFile(const String &directory, const String &fileName);

    /// @brief Creates a new directory (folder) on file the system.
    bool CreateDirectory(const String &directoryPath);

    /// @brief Creates a new directory (folder) on file the system.
    bool CreateDirectory(const String &where, const String &directoryName);

    /// @brief Removes a given directory (folder) from the file system.
    bool RemoveDirectory(const String &directoryPath);

    /// @brief Removes a given directory (folder) from the file system.
    bool RemoveDirectory(const String &where, const String &directoryName);

    /// @brief Creates a new directory (folder) on file the system, regardless of whether there already is one with the same path (and overwrites it).
    bool OverwriteDirectory(const String &directoryPath);

    /// @brief Creates a new directory (folder) on file the system, regardless of whether there already is one with the same path (and overwrites it).
    bool OverwriteDirectory(const String &where, const String &directoryName);

    /// @brief Returns a vector of strings, each representing a path of some of the subdirectories through which the specified path has gone.
    [[nodiscard]] std::vector<String> GetDirectoriesFromPath(const String &path);

    /// @brief Creates all needed directories, in order to make a new path available.
    /// @param targetPath Must follow this template: "SomeDirectory/SomeSubdirectory/OtherSubdirectory/".
    bool CreateDirectoriesToPath(const String &targetPath);

    /// @brief Returns only the last directory/file from a given path.
    /// @param filePath Which file path to use.
    [[nodiscard]] String GetFileNameFromPath(const String &filePath, bool includeExtension = true);

    /// @brief Returns a given file path without the file tag and extension at the end.
    /// @param filePath What file path to use.
    [[nodiscard]] String RemoveFileNameFromPath(const String &filePath);

    /// @brief Gets the file extension of a file in a given path.
    /// @param filePath Path to the file on system.
    /// @param includeDot Whether to add the '.' symbol at the beginning of the returned value. False by default.
    [[nodiscard]] String GetFileExtensionFromPath(const String &filePath, bool includeDot = false);

    /// @brief Searches for a given file tag within a directory and all of its subdirectories. Returns empty string if file not found.
    /// @param directory Which directory to search.</param>
    /// @param fileName Which file to search for. Must contain an extension!</param>
    [[nodiscard]] String FindInSubdirectories(const String &directory, const String &fileName);

    /* --- POLLING METHODS --- */
    void Start();
}