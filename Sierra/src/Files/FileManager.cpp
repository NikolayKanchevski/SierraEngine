//
// Created by Nikolay Kanchevski on 27.12.23.
//

#include "FileManager.h"

#include "PathErrors.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    FileType FilePathToFileType(const std::filesystem::path& filePath) noexcept
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

    /* --- POLLING METHODS --- */

    void FileManager::CreateFile(const std::filesystem::path& filePath, const FilePathConflictPolicy conflictPolicy) const
    {

    }

    void FileManager::RenameFile(const std::filesystem::path& filePath, const std::string_view name) const
    {
        SR_THROW_IF(!FileExists(filePath), PathMissingError("Cannot rename file at path", filePath));
        SR_THROW_IF(FileExists(filePath.parent_path() / name), PathAlreadyExistsError(SR_FORMAT("Cannot rename file to [{0}], as another file at the same location already exists", name), filePath.parent_path() / name));
    }

    void FileManager::CopyFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        SR_THROW_IF(!FileExists(sourceFilePath), PathMissingError("Cannot copy file", sourceFilePath));
    }

    void FileManager::MoveFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const
    {
        SR_THROW_IF(!FileExists(sourceFilePath), PathMissingError("Cannot move file", sourceFilePath));
    }

    void FileManager::DeleteFile(const std::filesystem::path& filePath) const
    {
        SR_THROW_IF(!FileExists(filePath), PathMissingError("Cannot delete file at path", filePath));
    }

    void FileManager::EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, const bool recursive, const FileEnumerationPredicate& Predicate) const
    {
        SR_THROW_IF(!DirectoryExists(directoryPath), PathMissingError("Cannot enumerate files in directory", directoryPath));
    }

    void FileManager::CreateDirectory(const std::filesystem::path& directoryPath) const
    {

    }

    void FileManager::RenameDirectory(const std::filesystem::path& directoryPath, const std::string_view name) const
    {
        SR_THROW_IF(!DirectoryExists(directoryPath), PathMissingError("Cannot rename directory at path", directoryPath));
        SR_THROW_IF(FileExists(directoryPath.parent_path() / name), PathAlreadyExistsError(SR_FORMAT("Cannot rename directory to [{0}], as another directory at the same location already exists", name), directoryPath.parent_path() / name));
    }

    void FileManager::CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const
    {
        SR_THROW_IF(!DirectoryExists(sourceDirectoryPath), PathMissingError("Cannot copy directory", sourceDirectoryPath));
    }

    void FileManager::MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, FilePathConflictPolicy conflictPolicy) const
    {
        SR_THROW_IF(!DirectoryExists(sourceDirectoryPath), PathMissingError("Cannot move directory", sourceDirectoryPath));
    }

    void FileManager::DeleteDirectory(const std::filesystem::path& directoryPath) const
    {
        SR_THROW_IF(!DirectoryExists(directoryPath), PathMissingError("Cannot delete directory at path", directoryPath));
    }


    /* --- PROTECTED METHODS --- */

    void FileManager::ResolveFilePathConflict(const std::filesystem::path& sourceFilePath, std::filesystem::path& destinationFilePath, const FilePathConflictPolicy conflictPolicy) const
    {
        if (FileExists(destinationFilePath))
        {
            switch (conflictPolicy)
            {
                case FilePathConflictPolicy::Overwrite:
                {
                    DeleteFile(destinationFilePath);
                    break;
                }
                case FilePathConflictPolicy::KeepExisting:
                {
                    return;
                }
                case FilePathConflictPolicy::KeepBoth:
                {
                    uint32 copyIndex = 0;
                    do
                    {
                        destinationFilePath.replace_filename(SR_FORMAT("{0}_{1}{2}", sourceFilePath.stem().string(), copyIndex, sourceFilePath.extension().string()));
                        copyIndex++;
                    } while (FileExists(destinationFilePath));
                    break;
                }
            }
        }
    }

    void FileManager::ResolveDirectoryPathConflict(const std::filesystem::path& sourceDirectoryPath, std::filesystem::path& destinationDirectoryPath, Sierra::FilePathConflictPolicy conflictPolicy) const
    {
        if (DirectoryExists(destinationDirectoryPath))
        {
            switch (conflictPolicy)
            {
                case FilePathConflictPolicy::Overwrite:
                {
                    DeleteFile(destinationDirectoryPath);
                    break;
                }
                case FilePathConflictPolicy::KeepExisting:
                {
                    return;
                }
                case FilePathConflictPolicy::KeepBoth:
                {
                    uint32 copyIndex = 0;
                    do
                    {
                        destinationDirectoryPath.replace_filename(SR_FORMAT("{0}_{1}", sourceDirectoryPath.stem().string(), copyIndex));
                        copyIndex++;
                    } while (DirectoryExists(destinationDirectoryPath));
                    break;
                }
            }
        }
    }

}