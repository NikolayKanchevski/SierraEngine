//
// Created by Nikolay Kanchevski on 27.12.23.
//

#pragma once

#include "../Rendering/Image.h"

namespace Sierra
{

    enum class FileWriteFlags : uint8
    {
        None          = 0x0000,
        Create        = 0x0001,
        Overwrite     = 0x0002,
    };
    SR_DEFINE_ENUM_FLAG_OPERATORS(FileWriteFlags);

    class SIERRA_API File final
    {
    public:
        /* --- POLLING METHODS --- */
        static bool CreateFile(const std::filesystem::path &filePath);
        static bool CreateDirectory(const std::filesystem::path &directoryPath);

        static bool DeleteFile(const std::filesystem::path &filePath);
        static bool DeleteDirectory(const std::filesystem::path &directoryPath);

        [[nodiscard]] static std::vector<uint8> ReadFile(const std::filesystem::path &filePath, uint64 sourceByteOffset = 0, uint64 memoryRange = 0);
        static bool WriteToFile(const std::filesystem::path &filePath, const void* memory, uint64 memoryRange, uint64 sourceByteOffset = 0, uint64 destinationByteOffset = 0, FileWriteFlags writeFlags = FileWriteFlags::Create);

        /* --- GETTER METHODS --- */
        [[nodiscard]] static bool FileExists(const std::filesystem::path &filePath);
        [[nodiscard]] static bool DirectoryExists(const std::filesystem::path &directoryPath);

        [[nodiscard]] static const std::filesystem::path& GetResourcesDirectoryPath();
        [[nodiscard]] static const std::filesystem::path& GetCachesDirectoryPath();
        [[nodiscard]] static const std::filesystem::path& GetTemporaryDirectoryPath();

    };

}
