//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "../Rendering/Image.h"

namespace Sierra
{

    enum class FileAccess : uint8
    {
        ReadOnly,
        WriteOnly,
        ReadWrite
    };

    class SIERRA_API File final
    {
    public:
        /* --- POLLING METHODS --- */
        void Seek(const uint64 index);
        void Resize(const uint64 memorySize);

        std::vector<uint8> Read(const uint64 memorySize = 0);
        void Write(const void* data, const uint64 memorySize);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline const std::filesystem::path& GetPath() const { return path; }
        [[nodiscard]] inline uint64 GetSize() const { return std::filesystem::file_size(path); }

        /* --- OPERATORS --- */
        File(const File&) = delete;
        File& operator=(const File&) = delete;

        /* --- MOVE SEMANTICS --- */
        File(File&& other) = default;
        File& operator=(File&& other) = default;

        /* --- DESTRUCTOR --- */
        ~File() = default;

    private:
        std::filesystem::path path;
        std::fstream stream;

        friend class FileManager;
        explicit File(const std::filesystem::path &path, std::fstream&& stream);

    };

    struct FileManagerCreateInfo
    {

    };

    class SIERRA_API FileManager final
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<File> OpenFile(const std::filesystem::path &filePath, FileAccess access = FileAccess::ReadWrite) const;
        [[nodiscard]] std::optional<File> CreateAndOpenFile(const std::filesystem::path &filePath, FileAccess access = FileAccess::ReadWrite) const;

        [[nodiscard]] bool FileExists(const std::filesystem::path &filePath) const;
        bool CreateFile(const std::filesystem::path &filePath) const;
        bool CopyFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path &destinationDirectory, bool overwrite = false) const;
        bool MoveFile(const std::filesystem::path &sourceFilePath, const std::filesystem::path &destinationDirectory, bool overwrite = false) const;
        bool DeleteFile(const std::filesystem::path &filePath) const;

        [[nodiscard]] bool DirectoryExists(const std::filesystem::path &directoryPath) const;
        bool CreateDirectory(const std::filesystem::path &directoryPath) const;
        bool CopyDirectory(const std::filesystem::path &sourceDirectory, const std::filesystem::path &destinationDirectory, bool overwrite = false) const;
        bool MoveDirectory(const std::filesystem::path &sourceDirectory, const std::filesystem::path &destinationDirectory, bool overwrite = false) const;
        bool DeleteDirectory(const std::filesystem::path &directoryPath) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] const std::filesystem::path& GetResourcesDirectoryPath() const;
        [[nodiscard]] const std::filesystem::path& GetCachesDirectoryPath() const;
        [[nodiscard]] const std::filesystem::path& GetTemporaryDirectoryPath() const;

        /* --- OPERATORS --- */
        FileManager(const FileManager&) = delete;
        FileManager& operator=(const FileManager&) = delete;

        /* --- DESTRUCTOR --- */
        ~FileManager() = default;

    private:
        friend class Application;
        explicit FileManager(const FileManagerCreateInfo &createInfo);

    };

}
