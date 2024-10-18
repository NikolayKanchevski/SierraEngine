//
// Created by Nikolay Kanchevski on 27.06.24.
//

#include "FoundationFileManager.h"

#include "../FileErrors.h"
#include "../PathErrors.h"
#include "NSFileErrorHandler.h"
#include "FoundationFileStream.h"

namespace Sierra
{

    /* --- CONVERSIONS --- */

    NSURL* PathToNSURL(const std::filesystem::path& path) noexcept
    {
        const std::string pathString = path.string();
        return [[NSURL alloc] initFileURLWithPath: [NSString stringWithCString: pathString.c_str() length: pathString.size()]];
    }

    /* --- CONSTRUCTORS --- */

    FoundationFileManager::FoundationFileManager(const FoundationFileManagerCreateInfo &createInfo)
        : fileManager(createInfo.fileManager)
    {
        SR_THROW_IF(createInfo.fileManager == nil, InvalidValueError("Cannot create Foundation file manager, as specified file manager must not be nil"));
    }

    /* --- POLLING METHODS --- */

    bool FoundationFileManager::FileExists(const std::filesystem::path& filePath) const noexcept
    {
        const std::string path = filePath.string();
        return [fileManager fileExistsAtPath: [NSString stringWithCString: path.c_str() length: path.size()]];
    }

    std::unique_ptr<FileStream> FoundationFileManager::OpenFileStream(const std::filesystem::path& filePath, const FileStreamAccess access, const FileStreamBuffering buffering) const
    {
        SR_THROW_IF(!FileExists(filePath), PathMissingError("Cannot open file stream, as the specified file path does not exist", filePath));
        NSURL* const URL = PathToNSURL(filePath);

        NSError* error = nil;
        NSFileHandle* fileHandle = nil;
        switch (access)
        {
            case FileStreamAccess::ReadOnly:      { fileHandle = [NSFileHandle fileHandleForReadingFromURL: URL error: &error]; break; }
            case FileStreamAccess::WriteOnly:     { fileHandle = [NSFileHandle fileHandleForWritingToURL: URL error: &error];   break; }
            case FileStreamAccess::ReadWrite:     { fileHandle = [NSFileHandle fileHandleForUpdatingURL: URL error: &error];    break; }
        }
        [URL release];

        // NOTE: From the tests I performed, no POSIX/fcntl/C-API buffering configuration causes any difference in speed at all on Apple platforms, which is why no accounting for buffering is done here

        if (error != nil) HandleNSFileError(error, "Could not open file stream", filePath);
        return std::make_unique<FoundationFileStream>(filePath, fileHandle);
    }

    void FoundationFileManager::CreateFile(const std::filesystem::path& filePath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CreateFile(filePath, conflictPolicy);

        std::filesystem::path resolvedFilePath = filePath;
        ResolveFilePathConflict(filePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(resolvedFilePath.parent_path());

        const std::string path = resolvedFilePath.string();
        BOOL success = [fileManager createFileAtPath: [NSString stringWithCString: path.c_str() length: path.size()] contents: [NSData data] attributes: nil];
        SR_THROW_IF(!success, UnknownFileError("Could not create file", filePath));
    }

    void FoundationFileManager::RenameFile(const std::filesystem::path& filePath, const std::string_view name) const
    {
        FileManager::RenameFile(filePath, name);

        NSURL* const sourceURL = PathToNSURL(filePath);
        NSURL* const destinationURL = PathToNSURL(filePath.parent_path() / name);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not rename file to [{0}]", name), filePath);
    }

    void FoundationFileManager::CopyFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CopyFile(sourceFilePath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedFilePath = destinationDirectoryPath / sourceFilePath.filename();
        ResolveFilePathConflict(sourceFilePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        NSURL* const sourceURL = PathToNSURL(sourceFilePath);
        NSURL* const destinationURL = PathToNSURL(resolvedFilePath);

        NSError* error = nil;
        [fileManager copyItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not copy file to [{0}]", resolvedFilePath.string()), sourceFilePath);
    }

    void FoundationFileManager::MoveFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::MoveFile(sourceFilePath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedFilePath = destinationDirectoryPath / sourceFilePath.filename();
        ResolveFilePathConflict(sourceFilePath, resolvedFilePath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        NSURL* const sourceURL = PathToNSURL(sourceFilePath);
        NSURL* const destinationURL = PathToNSURL(resolvedFilePath);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not move file to [{0}]", resolvedFilePath.string()), sourceFilePath);
    }

    void FoundationFileManager::DeleteFile(const std::filesystem::path& filePath) const
    {
        FileManager::DeleteFile(filePath);
        NSURL* const URL = PathToNSURL(filePath);

        NSError* error = nullptr;
        [fileManager removeItemAtURL: URL error: &error];
        [URL release];

        if (error != nil) HandleNSFileError(error, "Could not delete file", filePath);
    }

    bool FoundationFileManager::DirectoryExists(const std::filesystem::path& directoryPath) const noexcept
    {
        const std::string path = directoryPath.string();
        return [fileManager fileExistsAtPath: [NSString stringWithUTF8String: path.c_str()]];
    }

    void FoundationFileManager::EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, const bool recursive, const FileEnumerationPredicate& Predicate) const
    {
        FileManager::EnumerateDirectoryFiles(directoryPath, recursive, Predicate);

        if (!recursive)
        {
            for (const std::filesystem::path& path : std::filesystem::directory_iterator(directoryPath))
            {
                if (!std::filesystem::is_regular_file(path)) continue;
                Predicate(path);
            }
        }
        else
        {
            for (const std::filesystem::path& path : std::filesystem::recursive_directory_iterator(directoryPath))
            {
                if (!std::filesystem::is_regular_file(path)) continue;
                Predicate(path);
            }
        }
    }

    void FoundationFileManager::CreateDirectory(const std::filesystem::path& directoryPath) const
    {
        FileManager::CreateDirectory(directoryPath);
        if (DirectoryExists(directoryPath)) return;

        NSURL* const URL = PathToNSURL(directoryPath);

        NSError* error = nil;
        [fileManager createDirectoryAtURL: URL withIntermediateDirectories: YES attributes: nil error: &error];
        [URL release];

        if (error != nil) HandleNSFileError(error, "Could not create directory", directoryPath);
    }

    void FoundationFileManager::RenameDirectory(const std::filesystem::path& directoryPath, const std::string_view name) const
    {
        FileManager::RenameDirectory(directoryPath, name);

        NSURL* const sourceURL = PathToNSURL(directoryPath);
        NSURL* const destinationURL = PathToNSURL(directoryPath.parent_path() / name);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not rename directory to [{0}]", name), directoryPath);
    }

    void FoundationFileManager::CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::CopyDirectory(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedDirectoryPath = destinationDirectoryPath / sourceDirectoryPath.filename();
        ResolveDirectoryPathConflict(sourceDirectoryPath, resolvedDirectoryPath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        NSURL* const sourceURL = PathToNSURL(sourceDirectoryPath);
        NSURL* const destinationURL = PathToNSURL(resolvedDirectoryPath);

        NSError* error = nil;
        [fileManager copyItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not copy directory to [{0}]", resolvedDirectoryPath.string()), sourceDirectoryPath);
    }

    void FoundationFileManager::MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        FileManager::MoveDirectory(sourceDirectoryPath, destinationDirectoryPath, conflictPolicy);

        std::filesystem::path resolvedDirectoryPath = destinationDirectoryPath / sourceDirectoryPath.filename();
        ResolveDirectoryPathConflict(sourceDirectoryPath, resolvedDirectoryPath, conflictPolicy);
        CreateDirectory(destinationDirectoryPath);

        NSURL* const sourceURL = PathToNSURL(sourceDirectoryPath);
        NSURL* const destinationURL = PathToNSURL(resolvedDirectoryPath);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) HandleNSFileError(error, SR_FORMAT("Could not move directory to [{0}]", resolvedDirectoryPath.string()), sourceDirectoryPath);
    }

    void FoundationFileManager::DeleteDirectory(const std::filesystem::path& directoryPath) const
    {
        if (directoryPath.has_extension()) throw PathInvalidError("Cannot delete directory", directoryPath);
        NSURL* const URL = PathToNSURL(directoryPath);

        NSError* error = nullptr;
        [fileManager removeItemAtURL: URL error: &error];
        [URL release];

        if (error != nil) HandleNSFileError(error, "Could not delete directory", directoryPath);
    }

    /* --- GETTER METHODS --- */

    FileMetadata FoundationFileManager::GetFileMetadata(const std::filesystem::path& filePath) const
    {
        const std::string path = filePath.string();

        NSError* error = nil;
        const NSDictionary<NSFileAttributeKey, id>* const attributes = [fileManager attributesOfItemAtPath: [NSString stringWithCString: path.data() length: path.size()] error: &error];
        if (error != nil) HandleNSFileError(error, "Could not get file metadata", filePath);

        const NSNumber* const memorySize = [attributes objectForKey: NSFileSize];
        const NSDate* const dateCreated = [attributes objectForKey: NSFileCreationDate];
        const NSDate* const dateModified = [attributes objectForKey: NSFileModificationDate];

        const std::chrono::system_clock::time_point dateCreatedTimePoint(std::chrono::seconds(static_cast<uint64>(dateCreated.timeIntervalSince1970)));
        const std::chrono::system_clock::time_point dateModifiedTimePoint(std::chrono::seconds(static_cast<uint64>(dateModified.timeIntervalSince1970)));

        return
        {
            .type = FilePathToFileType(filePath),
            .memorySize = static_cast<uint64>([memorySize longLongValue]),
            .dateCreated = Date(TimePoint(dateCreatedTimePoint)),
            .dateLastModified = Date(TimePoint(dateModifiedTimePoint))
        };
    }

    DirectoryMetadata FoundationFileManager::GetDirectoryMetadata(const std::filesystem::path& directoryPath) const
    {
        const std::string path = directoryPath.string();

        NSError* error = nil;
        const NSDictionary<NSFileAttributeKey, id>* const attributes = [fileManager attributesOfItemAtPath: [NSString stringWithCString: path.data() length: path.size()] error: &error];
        if (error != nil) HandleNSFileError(error, "Could not get directory metadata", directoryPath);

        const NSDate* const dateCreated = [attributes objectForKey: NSFileCreationDate];
        const NSDate* const dateModified = [attributes objectForKey: NSFileModificationDate];

        const std::chrono::system_clock::time_point dateCreatedTimePoint(std::chrono::seconds(static_cast<uint64>(dateCreated.timeIntervalSince1970)));
        const std::chrono::system_clock::time_point dateModifiedTimePoint(std::chrono::seconds(static_cast<uint64>(dateModified.timeIntervalSince1970)));

        uint32 fileCount = 0;
        size memorySize = 0;
        for (const std::filesystem::path& filePath : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            if (!std::filesystem::is_regular_file(filePath)) continue;
            memorySize += std::filesystem::file_size(filePath);
            fileCount++;
        }

        return
        {
            .fileCount = fileCount,
            .memorySize = memorySize,
            .dateCreated = Date(TimePoint(dateCreatedTimePoint)),
            .dateLastModified = Date(TimePoint(dateModifiedTimePoint))
        };
    }

    std::filesystem::path FoundationFileManager::GetApplicationDirectoryPath() const noexcept
    {
        return { std::string_view(NSBundle.mainBundle.bundlePath.UTF8String, NSBundle.mainBundle.bundlePath.length) };
    }

    std::filesystem::path FoundationFileManager::GetExecutableDirectoryPath() const noexcept
    {
        return { std::string_view(NSBundle.mainBundle.executablePath.UTF8String, NSBundle.mainBundle.executablePath.length) };
    }

    std::filesystem::path FoundationFileManager::GetResourcesDirectoryPath() const noexcept
    {
        return { std::string_view(NSBundle.mainBundle.executablePath.UTF8String, NSBundle.mainBundle.resourcePath.length) };
    }

    std::filesystem::path FoundationFileManager::GetHomeDirectoryPath() const noexcept
    {
        return { std::string_view(NSHomeDirectory().UTF8String, NSHomeDirectory().length) };
    }

    std::filesystem::path FoundationFileManager::GetUserDirectoryPath() const noexcept
    {
        // NOTE: Only macOS has user folders
        #if SR_PLATFORM_macOS
            return { std::string_view(fileManager.homeDirectoryForCurrentUser.path.UTF8String, fileManager.homeDirectoryForCurrentUser.path.length) };
        #else
            return GetDocumentsDirectoryPath();
        #endif
    }

    std::filesystem::path FoundationFileManager::GetTemporaryDirectoryPath() const noexcept
    {
        return { std::string_view(NSTemporaryDirectory().UTF8String, NSTemporaryDirectory().length) };
    }

    std::filesystem::path FoundationFileManager::GetCachesDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSCachesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetDesktopDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSDesktopDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetDownloadsDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSDownloadsDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetDocumentsDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSDocumentDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetPicturesDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSPicturesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetMusicDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSMusicDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetVideosDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSMoviesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

}