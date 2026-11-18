//
// Created by Nikolay Kanchevski on 27.06.24.
//

#include "FoundationFileManager.h"

#include "FoundationFileStream.h"

namespace Sierra
{

    namespace
    {
        NSURL* PathToNSURL(const Path& path) noexcept
        {
            NSString* pathString = [NSString stringWithCString: path.GetString().data() encoding: NSUTF8StringEncoding];
            return [[NSURL alloc] initFileURLWithPath: pathString];
        }

        PathResult NSErrorToPathOperationResult(const NSError* error)
        {
            if (error == nil || error.code == 0)
                return PathResult::Success;

            switch (error.code)
            {
                case NSFileLockingError:                            return PathResult::Locked;
                case NSFileReadNoPermissionError:
                case NSFileWriteNoPermissionError:                  return PathResult::PermissionDenied;
                case NSFileReadCorruptFileError:                    return PathResult::Corrupted;
                case NSFileNoSuchFileError:
                case NSFileReadNoSuchFileError:                     return PathResult::NotFound;
                case NSFileReadTooLargeError:
                case NSFileWriteOutOfSpaceError:                    return PathResult::SizeExceeded;
                case NSFileWriteFileExistsError:                    return PathResult::AlreadyExists;
                case NSFileWriteVolumeReadOnlyError:                return PathResult::ReadOnly;

                case NSFileReadUnknownError:
                case NSFileWriteUnknownError:
                case NSFileReadInvalidFileNameError:
                case NSFileReadInapplicableStringEncodingError:
                case NSFileReadUnsupportedSchemeError:
                case NSFileReadUnknownStringEncodingError:
                case NSFileWriteInvalidFileNameError:
                case NSFileWriteInapplicableStringEncodingError:
                case NSFileWriteUnsupportedSchemeError:
                default:                                            break;
            }

            return PathResult::Unknown;
        }
    }

    /* --- CONSTRUCTORS --- */

    FoundationFileManager::FoundationFileManager(const FoundationFileManagerCreateInfo &createInfo)
        : fileManager(createInfo.fileManager)
    {
        SR_THROW_IF(createInfo.fileManager == nil, InvalidValueError("Cannot create Foundation file manager, as specified file manager must not be nil"));
    }

    /* --- POLLING METHODS --- */

    bool FoundationFileManager::PathExists(const Path& path) const noexcept
    {
        const std::string pathString = path;
        return [fileManager fileExistsAtPath: [NSString stringWithCString: pathString.data() encoding: NSUTF8StringEncoding]];
    }

    PathResult FoundationFileManager::CreateFile(const Path& filePath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CreateFile(filePath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedFilePath = filePath;
        if (const PathResult result = ResolvePathConflict(filePath, conflictPolicy, resolvedFilePath); result != PathResult::Success)
            return result;

        if (const PathResult result = CreateDirectory(resolvedFilePath.GetParentPath(), PathConflictPolicy::Overwrite); result != PathResult::Success)
            return result;

        const std::string pathString = resolvedFilePath.GetString();
        NSString* path = [NSString stringWithCString: pathString.data() encoding: NSUTF8StringEncoding];

        const BOOL success = [fileManager createFileAtPath: path contents: [NSData data] attributes: nil];
        return success ? PathResult::Success : PathResult::Unknown;
    }

    PathResult FoundationFileManager::CreateDirectory(const Path& directoryPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CreateDirectory(directoryPath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedDirectoryPath = directoryPath;
        if (const PathResult result = ResolvePathConflict(directoryPath, conflictPolicy, resolvedDirectoryPath); result != PathResult::Success)
            return result;

        NSURL* URL = PathToNSURL(directoryPath);

        NSError* error = nil;
        [fileManager createDirectoryAtURL: URL withIntermediateDirectories: YES attributes: nil error: &error];
        [URL release];

        return NSErrorToPathOperationResult(error);
    }

    PathResult FoundationFileManager::CopyPath(const Path& sourcePath, const Path& destinationPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::CopyPath(sourcePath, destinationPath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedPath = destinationPath / sourcePath.GetFileName();
        if (const PathResult result = ResolvePathConflict(sourcePath, conflictPolicy, resolvedPath); result != PathResult::Success)
            return result;

        if (IsFile(sourcePath))
        {
            if (const PathResult result = CreateFile(resolvedPath, PathConflictPolicy::Overwrite); result != PathResult::Success)
                return result;
        }
        else if (IsDirectory(sourcePath))
        {
            if (const PathResult result = CreateDirectory(resolvedPath, PathConflictPolicy::Overwrite); result != PathResult::Success)
                return result;
        }
        else
        {
            return PathResult::Unknown;
        }

        NSURL* sourceURL = PathToNSURL(sourcePath);
        NSURL* destinationURL = PathToNSURL(resolvedPath);

        NSError* error = nil;
        [fileManager copyItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        return NSErrorToPathOperationResult(error);
    }

    PathResult FoundationFileManager::RenamePath(const Path& path, const std::string_view name) const
    {
        if (const PathResult result = FileManager::RenamePath(path, name); result != PathResult::Success)
            return result;

        NSURL* sourceURL = PathToNSURL(path);
        NSURL* destinationURL = PathToNSURL(path.GetParentPath() / name);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        return NSErrorToPathOperationResult(error);
    }

    PathResult FoundationFileManager::MovePath(const Path& sourcePath, const Path& destinationPath, const PathConflictPolicy conflictPolicy) const
    {
        if (const PathResult result = FileManager::MovePath(sourcePath, destinationPath, conflictPolicy); result != PathResult::Success)
            return result;

        Path resolvedPath = destinationPath / sourcePath.GetFileName();
        if (const PathResult result = ResolvePathConflict(sourcePath, conflictPolicy, resolvedPath); result != PathResult::Success)
            return result;

        if (IsFile(sourcePath))
        {
            if (const PathResult result = CreateFile(resolvedPath, PathConflictPolicy::Overwrite); result != PathResult::Success)
                return result;
        }
        else if (IsDirectory(sourcePath))
        {
            if (const PathResult result = CreateDirectory(resolvedPath, PathConflictPolicy::Overwrite); result != PathResult::Success)
                return result;
        }
        else
        {
            return PathResult::Unknown;
        }

        NSURL* sourceURL = PathToNSURL(sourcePath);
        NSURL* destinationURL = PathToNSURL(resolvedPath);

        NSError* error = nil;
        [fileManager moveItemAtURL: sourceURL toURL: destinationURL error: &error];

        [sourceURL release];
        [destinationURL release];

        return NSErrorToPathOperationResult(error);
    }

    PathResult FoundationFileManager::DeletePath(const Path& path) const
    {
        if (const PathResult result = FileManager::DeletePath(path); result != PathResult::Success)
            return result;

        NSURL* URL = PathToNSURL(path);

        NSError* error = nullptr;
        [fileManager removeItemAtURL: URL error: &error];
        [URL release];

        return NSErrorToPathOperationResult(error);
    }

    PathResult FoundationFileManager::OpenFileStream(const FileStreamCreateInfo& createInfo, std::unique_ptr<FileStream>& outFileStream) const
    {
        if (!PathExists(createInfo.filePath) || !IsFile(createInfo.filePath))
            return PathResult::NotFound;

        NSURL* URL = PathToNSURL(createInfo.filePath);
        NSFileHandle* fileHandle = nil;

        NSError* error = nil;
        switch (createInfo.access)
        {
            case StreamAccess::ReadOnly:      { fileHandle = [NSFileHandle fileHandleForReadingFromURL: URL error: &error]; break; }
            case StreamAccess::WriteOnly:     { fileHandle = [NSFileHandle fileHandleForWritingToURL: URL error: &error];   break; }
            case StreamAccess::ReadWrite:     { fileHandle = [NSFileHandle fileHandleForUpdatingURL: URL error: &error];    break; }
        }
        [URL release];

        if (const PathResult result = NSErrorToPathOperationResult(error); result != PathResult::Success)
            return result;

        // NOTE: From the tests I performed, no POSIX/fcntl/C-API buffering configuration results in any performance difference on Apple platforms, which is why no accounting for buffering is done here
        outFileStream = std::make_unique<FoundationFileStream>(fileHandle, createInfo);
        return PathResult::Success;
    }

    PathResult FoundationFileManager::EnumeratePaths(const Path& path, const PathEnumerationPredicate& Predicate) const
    {
        if (const PathResult result = FileManager::EnumeratePaths(path, Predicate); result != PathResult::Success)
            return result;

        if (!IsDirectory(path))
        {
            Predicate(path);
            return PathResult::Success;
        }

        std::filesystem::recursive_directory_iterator iterator(path);
        const std::filesystem::recursive_directory_iterator end;

        for (; iterator != end; ++iterator)
        {
            if (Predicate(iterator->path()))
            {
                iterator.disable_recursion_pending();
            }
        }

        return PathResult::Success;
    }

    /* --- GETTER METHODS --- */

    bool FoundationFileManager::IsFile(const Path& path) const
    {
        const std::string pathString = path;

        BOOL isDirectory = NO;
        return [fileManager fileExistsAtPath: [NSString stringWithCString: pathString.data() encoding: NSUTF8StringEncoding] isDirectory: &isDirectory] && !isDirectory;
    }

    bool FoundationFileManager::IsDirectory(const Path& path) const
    {
        const std::string pathString = path;

        BOOL isDirectory = NO;
        return [fileManager fileExistsAtPath: [NSString stringWithCString: pathString.data() encoding: NSUTF8StringEncoding] isDirectory: &isDirectory] && isDirectory;
    }

    PathResult FoundationFileManager::GetPathMetadata(const Path& path, PathMetadata& outMetadata) const
    {
        const NSURL* URL = PathToNSURL(path);

        NSError* error = nil;
        const NSDictionary<NSFileAttributeKey, id>* const attributes = [fileManager attributesOfItemAtPath: URL.path error: &error];

        if (const PathResult result = NSErrorToPathOperationResult(error); result != PathResult::Success)
        {
            [URL release];
            return result;
        }

        const NSDate* const dateCreated = [attributes objectForKey: NSFileCreationDate];
        const NSDate* const dateModified = [attributes objectForKey: NSFileModificationDate];

        const std::chrono::system_clock::time_point timeCreatedTimePoint(std::chrono::system_clock::time_point(std::chrono::seconds(static_cast<uint64>(dateCreated.timeIntervalSince1970))));
        const std::chrono::system_clock::time_point timeModifiedTimePoint(std::chrono::system_clock::time_point(std::chrono::seconds(static_cast<uint64>(dateModified.timeIntervalSince1970))));

        outMetadata =
        {
            .itemCount = 1,
            .memorySize = [[attributes objectForKey: NSFileSize] unsignedLongValue],
            .timeCreated = TimePoint(timeCreatedTimePoint),
            .timeLastModified = TimePoint(timeModifiedTimePoint)
        };

        if ([[attributes objectForKey: NSFileTypeDirectory] boolValue])
        {
            NSArray<NSString*>* contents = [fileManager contentsOfDirectoryAtPath: URL.path error: &error];
            if (contents != nil) outMetadata.itemCount = [contents count];
        }
        [URL release];

        return PathResult::Success;
    }

    Path FoundationFileManager::GetApplicationDirectoryPath() const noexcept
    {
        return { std::string_view(NSBundle.mainBundle.bundlePath.UTF8String, NSBundle.mainBundle.bundlePath.length) };
    }

    Path FoundationFileManager::GetExecutableDirectoryPath() const noexcept
    {
        return { std::string_view(NSBundle.mainBundle.executablePath.UTF8String, NSBundle.mainBundle.executablePath.length) };
    }

    Path FoundationFileManager::GetResourcesDirectoryPath() const noexcept
    {
        return { std::string_view(NSBundle.mainBundle.executablePath.UTF8String, NSBundle.mainBundle.resourcePath.length) };
    }

    Path FoundationFileManager::GetHomeDirectoryPath() const noexcept
    {
        return { std::string_view(NSHomeDirectory().UTF8String, NSHomeDirectory().length) };
    }

    Path FoundationFileManager::GetUserDirectoryPath() const noexcept
    {
        // NOTE: Only macOS has user folders
        #if SR_PLATFORM_macOS
            return { std::string_view(fileManager.homeDirectoryForCurrentUser.path.UTF8String, fileManager.homeDirectoryForCurrentUser.path.length) };
        #else
            return GetDocumentsDirectoryPath();
        #endif
    }

    Path FoundationFileManager::GetTemporaryDirectoryPath() const noexcept
    {
        return { std::string_view(NSTemporaryDirectory().UTF8String, NSTemporaryDirectory().length) };
    }

    Path FoundationFileManager::GetCachesDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSCachesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    Path FoundationFileManager::GetDesktopDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSDesktopDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    Path FoundationFileManager::GetDownloadsDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSDownloadsDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    Path FoundationFileManager::GetDocumentsDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSDocumentDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    Path FoundationFileManager::GetPicturesDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSPicturesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    Path FoundationFileManager::GetMusicDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSMusicDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }

    Path FoundationFileManager::GetVideosDirectoryPath() const noexcept
    {
        NSURL* const URL = [fileManager URLForDirectory: NSMoviesDirectory inDomain: NSUserDomainMask appropriateForURL: [NSURL fileURLWithPath: fileManager.currentDirectoryPath] create: YES error: nil];
        return { std::string_view(URL.path.UTF8String, URL.path.length) };
    }
}