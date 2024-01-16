//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

namespace Sierra
{

    class SIERRA_API File
    {
    public:
        /* --- POLLING METHODS --- */
        static bool CreateFile(const std::filesystem::path &filePath);
        static bool CreateDirectory(const std::filesystem::path &directoryPath);

        static bool DeleteFile(const std::filesystem::path &filePath);
        static bool DeleteDirectory(const std::filesystem::path &directoryPath);

        [[nodiscard]] static std::vector<char> ReadFile(const std::filesystem::path &filePath);
        static bool WriteToFile(const std::filesystem::path &filePath, const char* data, uint64 dataSize, bool overwrite, bool createFile = true);

        /* --- GETTER METHODS --- */
        [[nodiscard]] static bool FileExists(const std::filesystem::path &filePath);
        [[nodiscard]] static bool DirectoryExists(const std::filesystem::path &directoryPath);

        [[nodiscard]] static const std::filesystem::path& GetResourcesDirectoryPath();
        [[nodiscard]] static const std::filesystem::path& GetCachesDirectoryPath();
        [[nodiscard]] static const std::filesystem::path& GetTemporaryDirectoryPath();

    };

}
