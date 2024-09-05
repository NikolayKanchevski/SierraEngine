//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "FileManager.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    FileType FileManager::FilePathToFileType(const std::filesystem::path& filePath)
    {
        if (!filePath.has_extension()) return FileType::Unknown;

        static const std::unordered_map<std::string_view, FileType> FILE_EXTENSION_TABLE =
        {
            { ".txt",       FileType::Text },
            { ".md",        FileType::Text },
            { ".rtf",       FileType::Text },
            { ".csv",       FileType::Text },
            { ".pdf",       FileType::Document },
            { ".doc",       FileType::Document },
            { ".docx",      FileType::Document },
            { ".xls",       FileType::Document },
            { ".xlsx",      FileType::Document },
            { ".ppt",       FileType::Document },
            { ".pptx",      FileType::Document },
            { ".jpg",       FileType::Image },
            { ".jpeg",      FileType::Image },
            { ".png",       FileType::Image },
            { ".gif",       FileType::Image },
            { ".bmp",       FileType::Image },
            { ".tiff",      FileType::Image },
            { ".svg",       FileType::Image },
            { ".webp",      FileType::Image },
            { ".svg",       FileType::Image },
            { ".mp3",       FileType::Audio },
            { ".wav",       FileType::Audio },
            { ".flac",      FileType::Audio },
            { ".ogg",       FileType::Audio },
            { ".aac",       FileType::Audio },
            { ".m4a",       FileType::Audio },
            { ".mp4",       FileType::Video },
            { ".avi",       FileType::Video },
            { ".mkv",       FileType::Video },
            { ".mov",       FileType::Video },
            { ".flv",       FileType::Video },
            { ".wmv",       FileType::Video },
            { ".webm",      FileType::Video },
            { ".zip",       FileType::Archive },
            { ".rar",       FileType::Archive },
            { ".tar",       FileType::Archive },
            { ".gz",        FileType::Archive },
            { ".bz2",       FileType::Archive },
            { ".7z",        FileType::Archive },
            { ".exe",       FileType::Executable },
            { ".dll",       FileType::Executable },
            { ".so",        FileType::Executable },
            { ".dmg",       FileType::Executable },
            { ".iso",       FileType::Executable },
            { ".ini",       FileType::Configuration },
            { ".conf",      FileType::Configuration },
            { ".config",    FileType::Configuration },
            { ".env",       FileType::Configuration },
            { ".xml",       FileType::Configuration },
            { ".json",      FileType::Configuration },
            { ".yaml",      FileType::Configuration },
        };

        const std::string extension = filePath.extension().string();
        const auto iterator = FILE_EXTENSION_TABLE.find(extension);

        if (iterator != FILE_EXTENSION_TABLE.end()) return iterator->second;
        return FileType::Unknown;
    }

}