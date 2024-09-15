//
// Created by Nikolay Kanchevski on 27.06.24.
//

#include "FoundationFileManager.h"

namespace Sierra
{

    namespace
    {
        FileOperationResult NSErrorToFileOperationResult(const NSError* const error)
        {
            switch (error.code)
            {
                case NSFileReadUnknownError:
                case NSFileWriteUnknownError:                            return FileOperationResult::UnknownError;
                case NSFileReadInvalidFileNameError:
                case NSFileWriteInvalidFileNameError:                    return FileOperationResult::FilePathInvalid;
                case NSFileReadNoSuchFileError:
                case NSFileNoSuchFileError:                              return FileOperationResult::NoSuchFilePath;
                case NSFileWriteFileExistsError:                         return FileOperationResult::FilePathAlreadyExists;
                case NSFileReadNoPermissionError:
                case NSFileWriteNoPermissionError:                       return FileOperationResult::FileAccessDenied;
                case NSFileLockingError:                                 return FileOperationResult::FileLocked;
                case NSFileReadCorruptFileError:                         return FileOperationResult::FileCorrupted;
                case NSFileWriteVolumeReadOnlyError:                     return FileOperationResult::FileReadOnly;
                case NSFileWriteOutOfSpaceError:                         return FileOperationResult::FileTooLarge;
                default:                                                 break;
            }

            return FileOperationResult::UnknownError;
        }
    }

    /* --- CONSTRUCTORS --- */

    FoundationFileStream::FoundationFileStream(NSFileHandle* const fileHandle)
        : FileStream(), fileHandle(fileHandle)
    {
        SR_ERROR_IF(fileHandle == nil, "Passed file handle must not be a null pointer!");
        SR_ERROR_IF(fileHandle.fileDescriptor == -1, "Passed file descriptor must be valid!");
    }

    FileOperationResult FoundationFileStream::Seek(const size offset)
    {
        NSError* error = nil;
        [fileHandle seekToOffset: offset error: &error];

        if (error != nil)
        {
            const FileOperationResult result = NSErrorToFileOperationResult(error);
            return result;
        }

        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileStream::Read(const size memorySize, std::vector<uint8>& outData)
    {
        NSError* error = nil;
        NSData* const data = [fileHandle readDataUpToLength: memorySize error: &error];

        if (error != nil)
        {
            const FileOperationResult result = NSErrorToFileOperationResult(error);
            return result;
        }

        outData.resize(memorySize);
        std::memcpy(outData.data(), data.bytes, memorySize);

        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileStream::Write(const void* memory, const size memorySize, const size offset)
    {
        NSData* const data = [[NSData dataWithBytesNoCopy: const_cast<void*>(reinterpret_cast<const uint8*>(memory) + offset) length: memorySize] retain];

        NSError* error = nil;
        [fileHandle writeData: data error: &error];

        if (error != nil)
        {
            const FileOperationResult result = NSErrorToFileOperationResult(error);
            return result;
        }

        return FileOperationResult::Success;
    }

    /* --- GETTER METHODS --- */

    size FoundationFileStream::GetMemorySize() const
    {
        size initialOffset = GetCurrentOffset();

        NSUInteger memorySize = 0;
        [fileHandle seekToEndReturningOffset: memorySize error: nil];

        Seek(initialOffset);
        return memorySize;
    }

    size FoundationFileStream::GetCurrentOffset() const
    {
        NSUInteger offset = 0;
        [fileHandle getOffset: offset error: nil];
        return offset;
    }

    /* --- DESTRUCTOR --- */

    FoundationFileStream::~FoundationFileStream()
    {
        [fileHandle closeFile];
    }

    /* --- CONSTRUCTORS --- */

    FoundationFileManager::FoundationFileManager()
        : FileManager(), fileManager([NSFileManager defaultManager])
    {

    }

    /* --- POLLING METHODS --- */

    bool FoundationFileManager::FileExists(const std::filesystem::path& filePath) const
    {
        const std::string filePathString = filePath.string();
        NSString* const path = [NSString stringWithUTF8String: filePathString.c_str()];
        return [fileManager fileExistsAtPath: path];
    }

    FileOperationResult FoundationFileManager::OpenFileStream(const std::filesystem::path& filePath, const FileStreamAccess access, const FileStreamBuffering buffering, std::unique_ptr<FileStream>& outFileStream) const
    {
        NSURL* const URL = PathToNSURL(filePath);

        NSError* error = nil;
        NSFileHandle* fileHandle = nil;
        switch (access)
        {
            case FileStreamAccess::ReadOnly:      { fileHandle = [NSFileHandle fileHandleForReadingFromURL: URL error: &error]; break; }
            case FileStreamAccess::WriteOnly:     { fileHandle = [NSFileHandle fileHandleForWritingToURL: URL error: &error];   break; }
            case FileStreamAccess::ReadWrite:     { fileHandle = [NSFileHandle fileHandleForUpdatingURL: URL error: &error]; break; }
        }
        [URL release];

        // Handle buffering
        int flags = fcntl(fileHandle.fileDescriptor, F_GETFL);
        switch (buffering)
        {
            case FileStreamBuffering::Buffered:   { flags &= ~F_NOCACHE; break; }
            case FileStreamBuffering::Unbuffered: { flags |= F_NOCACHE;  break; }
        }
        fcntl(fileHandle.fileDescriptor, F_SETFL, flags);

        if (error != nil)
        {
            const FileOperationResult result = NSErrorToFileOperationResult(error);
            return result;
        }

        outFileStream = std::make_unique<FoundationFileStream>(fileHandle);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::CreateFile(const std::filesystem::path& filePath) const
    {
        if (const FileOperationResult result = CreateDirectory(filePath.parent_path()); result != FileOperationResult::Success)
        {
            return result;
        }

        const std::string filePathString = filePath.string();
        NSString* const path = [NSString stringWithUTF8String: filePathString.c_str()];
        return [fileManager createFileAtPath: path contents: [NSData data] attributes: nil] ? FileOperationResult::Success : FileOperationResult::UnknownError;
    }

    FileOperationResult FoundationFileManager::RenameFile(const std::filesystem::path& filePath, const std::string_view name) const
    {

        NSURL* const sourceURL = PathToNSURL(filePath);
        NSURL* const destinationURL = PathToNSURL(filePath.parent_path() / name);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::CopyFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        std::filesystem::path destinationFilePath = destinationDirectoryPath / sourceFilePath.filename();
        if (const FileOperationResult result = ResolveFilePathConflict(sourceFilePath, destinationFilePath, conflictPolicy); result != FileOperationResult::Success)
        {
            return result;
        }

        if (const FileOperationResult result = CreateDirectory(destinationDirectoryPath); result != FileOperationResult::Success)
        {
            return result;
        }

        NSURL* const sourceURL = PathToNSURL(sourceFilePath);
        NSURL* const destinationURL = PathToNSURL(destinationFilePath);

        NSError* error = nil;
        [fileManager copyItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::MoveFile(const std::filesystem::path& sourceFilePath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        std::filesystem::path destinationFilePath = destinationDirectoryPath / sourceFilePath.filename();
        if (const FileOperationResult result = ResolveFilePathConflict(sourceFilePath, destinationFilePath, conflictPolicy); result != FileOperationResult::Success)
        {
            return result;
        }

        if (const FileOperationResult result = CreateDirectory(destinationDirectoryPath); result != FileOperationResult::Success)
        {
            return result;
        }

        NSURL* const sourceURL = PathToNSURL(sourceFilePath);
        NSURL* const destinationURL = PathToNSURL(destinationFilePath);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::DeleteFile(const std::filesystem::path& filePath) const
    {
        NSURL* const URL = PathToNSURL(filePath);

        NSError* error = nullptr;
        [fileManager removeItemAtURL: URL error: &error];
        [URL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    bool FoundationFileManager::DirectoryExists(const std::filesystem::path& directoryPath) const
    {
        const std::string directoryPathString = directoryPath.string();
        NSString* const path = [NSString stringWithUTF8String: directoryPathString.c_str()];
        return [fileManager fileExistsAtPath: path];
    }

    FileOperationResult FoundationFileManager::EnumerateDirectoryFiles(const std::filesystem::path& directoryPath, std::vector<std::filesystem::path>& outFiles, const bool recursive) const
    {
        if (!recursive)
        {
            for (const std::filesystem::path& path : std::filesystem::directory_iterator(directoryPath))
            {
                if (is_directory(path) || is_symlink(path)) continue;
                outFiles.emplace_back(path);
            }
        }
        else
        {
            for (const std::filesystem::path& path : std::filesystem::recursive_directory_iterator(directoryPath))
            {
                if (is_directory(path) || is_symlink(path)) continue;
                outFiles.emplace_back(path);
            }
        }

        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::CreateDirectory(const std::filesystem::path& directoryPath) const
    {
        NSURL* const URL = PathToNSURL(directoryPath);

        NSError* error = nil;
        [fileManager createDirectoryAtURL: URL withIntermediateDirectories: YES attributes: nil error: &error];
        [URL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::RenameDirectory(const std::filesystem::path& directoryPath, const std::string_view name) const
    {
        NSURL* const sourceURL = PathToNSURL(directoryPath);
        NSURL* const destinationURL = PathToNSURL(directoryPath.parent_path() / name);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::CopyDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        std::filesystem::path destinationFilePath = destinationDirectoryPath / sourceDirectoryPath.filename();
        if (const FileOperationResult result = ResolveFilePathConflict(sourceDirectoryPath, destinationFilePath, conflictPolicy); result != FileOperationResult::Success)
        {
            return result;
        }

        if (const FileOperationResult result = CreateDirectory(destinationDirectoryPath); result != FileOperationResult::Success)
        {
            return result;
        }

        NSURL* const sourceURL = PathToNSURL(sourceDirectoryPath);
        NSURL* const destinationURL = PathToNSURL(destinationFilePath);

        NSError* error = nil;
        [fileManager copyItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::MoveDirectory(const std::filesystem::path& sourceDirectoryPath, const std::filesystem::path& destinationDirectoryPath, const FilePathConflictPolicy conflictPolicy) const
    {
        std::filesystem::path destinationFilePath = destinationDirectoryPath / sourceDirectoryPath.filename();
        if (const FileOperationResult result = ResolveFilePathConflict(sourceDirectoryPath, destinationFilePath, conflictPolicy); result != FileOperationResult::Success)
        {
            return result;
        }

        if (const FileOperationResult result = CreateDirectory(destinationDirectoryPath); result != FileOperationResult::Success)
        {
            return result;
        }

        NSURL* const sourceURL = PathToNSURL(sourceDirectoryPath);
        NSURL* const destinationURL = PathToNSURL(destinationFilePath);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::DeleteDirectory(const std::filesystem::path& directoryPath) const
    {
        NSURL* const URL = PathToNSURL(directoryPath);

        NSError* error = nullptr;
        [fileManager removeItemAtURL: URL error: &error];
        [URL release];

        if (error != nil) return NSErrorToFileOperationResult(error);
        return FileOperationResult::Success;
    }

    /* --- GETTER METHODS --- */

    FileOperationResult FoundationFileManager::GetFileMetadata(const std::filesystem::path& filePath, FileMetadata& outMetadata) const
    {
        const std::string filePathString = filePath.string();
        NSString* const path = [[NSString alloc] initWithBytes: filePathString.data() length: filePathString.size() encoding: NSASCIIStringEncoding];

        NSError* error = nil;
        NSDictionary<NSFileAttributeKey, id>* attributes = [[fileManager attributesOfItemAtPath: path error: &error] retain];
        [path release];

        if (error != nil) return NSErrorToFileOperationResult(error);

        NSNumber* const memorySize = [attributes objectForKey: NSFileSize];
        NSDate* const dateCreated = [attributes objectForKey: NSFileCreationDate];
        NSDate* const dateModified = [attributes objectForKey: NSFileModificationDate];

        const std::chrono::system_clock::time_point dateCreatedTimePoint(std::chrono::seconds(static_cast<uint64>(dateCreated.timeIntervalSince1970)));
        const std::chrono::system_clock::time_point dateModifiedTimePoint(std::chrono::seconds(static_cast<uint64>(dateModified.timeIntervalSince1970)));

        outMetadata = {
            .type = FilePathToFileType(filePath),
            .memorySize = static_cast<uint64>([memorySize longLongValue]),
            .dateCreated = Date(TimePoint(dateCreatedTimePoint)),
            .dateLastModified = Date(TimePoint(dateModifiedTimePoint))
        };

        [attributes release];
        return FileOperationResult::Success;
    }

    FileOperationResult FoundationFileManager::GetDirectoryMetadata(const std::filesystem::path& directoryPath, DirectoryMetadata& outMetadata) const
    {
        const std::string directoryPathString = directoryPath.string();
        NSString* const path = [[NSString alloc] initWithBytes: directoryPathString.data() length: directoryPathString.size() encoding: NSASCIIStringEncoding];

        NSError* error = nil;
        NSDictionary<NSFileAttributeKey, id>* attributes = [[fileManager attributesOfItemAtPath: path error: &error] retain];
        [path release];

        if (error != nil) return NSErrorToFileOperationResult(error);

        NSDate* dateCreated = [attributes objectForKey: NSFileCreationDate];
        NSDate* dateModified = [attributes objectForKey: NSFileModificationDate];

        const std::chrono::system_clock::time_point dateCreatedTimePoint(std::chrono::seconds(static_cast<uint64>(dateCreated.timeIntervalSince1970)));
        const std::chrono::system_clock::time_point dateModifiedTimePoint(std::chrono::seconds(static_cast<uint64>(dateModified.timeIntervalSince1970)));

        uint32 fileCount = 0;
        size memorySize = 0;
        for (const std::filesystem::path& currentPath : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            if (is_directory(currentPath) || is_symlink(currentPath)) continue;
            memorySize += std::filesystem::file_size(currentPath);
            fileCount++;
        }

        outMetadata = {
            .fileCount = fileCount,
            .memorySize = memorySize,
            .dateCreated = Date(TimePoint(dateCreatedTimePoint)),
            .dateLastModified = Date(TimePoint(dateModifiedTimePoint))
        };

        [attributes release];
        return FileOperationResult::Success;
    }

    std::filesystem::path FoundationFileManager::GetApplicationDirectoryPath() const
    {
        return { std::string_view(NSBundle.mainBundle.bundlePath.UTF8String, NSBundle.mainBundle.bundlePath.length) };
    }

    std::filesystem::path FoundationFileManager::GetExecutableDirectoryPath() const
    {
        return { std::string_view(NSBundle.mainBundle.executablePath.UTF8String, NSBundle.mainBundle.executablePath.length) };
    }

    std::filesystem::path FoundationFileManager::GetResourcesDirectoryPath() const
    {
        return { std::string_view(NSBundle.mainBundle.executablePath.UTF8String, NSBundle.mainBundle.resourcePath.length) };
    }

    std::filesystem::path FoundationFileManager::GetHomeDirectoryPath() const
    {
        return { std::string_view(NSHomeDirectory().UTF8String, NSHomeDirectory().length) };
    }

    std::filesystem::path FoundationFileManager::GetUserDirectoryPath() const
    {
        // NOTE: Only macOS has user folders
        #if SR_PLATFORM_macOS
            return { std::string_view(fileManager.homeDirectoryForCurrentUser.path.UTF8String, fileManager.homeDirectoryForCurrentUser.path.length) };
        #else
            return GetDocumentsDirectoryPath();
        #endif
    }

    std::filesystem::path FoundationFileManager::GetTemporaryDirectoryPath() const
    {
        return { std::string_view(NSTemporaryDirectory().UTF8String, NSTemporaryDirectory().length) };
    }

    std::filesystem::path FoundationFileManager::GetCachesDirectoryPath() const
    {
        NSError* error = nil;
        NSURL* const URL = [fileManager URLForDirectory: NSCachesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: NO error: &error];

        SR_ERROR_IF(error != nil, "Could not retrieve Caches directory path!");
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetDesktopDirectoryPath() const
    {
        NSError* error = nil;
        NSURL* const URL = [fileManager URLForDirectory: NSDesktopDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: NO error: &error];

        SR_ERROR_IF(error != nil, "Could not retrieve Desktop directory path!");
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetDownloadsDirectoryPath() const
    {
        NSError* error = nil;
        NSURL* const URL = [fileManager URLForDirectory: NSDownloadsDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: NO error: &error];

        SR_ERROR_IF(error != nil, "Could not retrieve Downloads directory path!");
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetDocumentsDirectoryPath() const
    {
        NSError* error = nil;
        NSURL* const URL = [fileManager URLForDirectory: NSDocumentDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: NO error: &error];

        SR_ERROR_IF(error != nil, "Could not retrieve Documents directory path!");
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetPicturesDirectoryPath() const
    {
        NSError* error = nil;
        NSURL* const URL = [fileManager URLForDirectory: NSPicturesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: NO error: &error];

        SR_ERROR_IF(error != nil, "Could not retrieve Pictures directory path!");
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetMusicDirectoryPath() const
    {
        NSError* error = nil;
        NSURL* const URL = [fileManager URLForDirectory: NSMusicDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: NO error: &error];

        SR_ERROR_IF(error != nil, "Could not retrieve Music directory path!");
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    std::filesystem::path FoundationFileManager::GetVideosDirectoryPath() const
    {
        NSError* error = nil;
        NSURL* const URL = [fileManager URLForDirectory: NSMoviesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: NO error: &error];

        SR_ERROR_IF(error != nil, "Could not retrieve Videos directory path!");
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    /* --- CONVERSIONS --- */

    NSURL* FoundationFileManager::PathToNSURL(const std::filesystem::path& givenPath)
    {
        const std::string pathString = givenPath.string();
        NSString* const path = [[NSString alloc] initWithBytes: pathString.data() length: pathString.size() encoding: NSASCIIStringEncoding];

        NSURL* URL = [[NSURL alloc] initFileURLWithPath: path];
        [path release];

        return URL;
    }

}