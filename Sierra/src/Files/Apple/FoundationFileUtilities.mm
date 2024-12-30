//
// Created by Nikolay Kanchevski on 23.09.24.
//

#include "FoundationFileUtilities.h"

#include "../FileErrors.h"
#include "../PathErrors.h"

namespace Sierra
{

    NSURL* PathToNSURL(const std::filesystem::path& path) noexcept
    {
        const std::string pathString = path.string();
        return [[NSURL alloc] initFileURLWithPath: [NSString stringWithCString: pathString.c_str() length: pathString.size()]];
    }

    void HandleNSFileError(const NSError* const error, const std::string_view message, const std::filesystem::path& path)
    {
        if (error == nil || error.code == 0) return;

        switch (error.code)
        {
            case NSFileReadUnknownError:
            case NSFileWriteUnknownError:                            break;
            case NSFileReadInvalidFileNameError:
            case NSFileWriteInvalidFileNameError:                    throw PathInvalidError(message, path);
            case NSFileReadNoSuchFileError:
            case NSFileNoSuchFileError:                              throw PathMissingError(message, path);
            case NSFileWriteFileExistsError:                         throw PathAlreadyExistsError(message, path);
            case NSFileReadNoPermissionError:
            case NSFileWriteNoPermissionError:                       throw FileAccessDeniedError(message, path);
            case NSFileLockingError:                                 throw FileLockedError(message, path);
            case NSFileReadCorruptFileError:                         throw FileCorruptedError(message, path);
            case NSFileWriteVolumeReadOnlyError:                     throw FileReadOnlyError(message, path);
            case NSFileWriteOutOfSpaceError:                         throw FileTooLargeError(message, path);
            default:                                                 break;
        }

        throw UnknownFileError(message, path);
    }

}