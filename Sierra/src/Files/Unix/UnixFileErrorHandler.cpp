//
// Created by Nikolay Kanchevski on 10.17.24.
//

#include "UnixFileErrorHandler.h"

#include "../FileErrors.h"
#include "../PathErrors.h"

namespace Sierra
{

    void HandleUnixFileError(const int errorCode, const std::string_view message, const std::filesystem::path& path)
    {
        if (errorCode == 0) return;

        switch (errorCode)
        {
            case EBADF:
            case ENOENT:        throw PathMissingError(message, path);
            case EEXIST:        throw PathAlreadyExistsError(message, path);
            case EACCES:        throw FileAccessDeniedError(message, path);
            case ETXTBSY:       throw FileLockedError(message, path);
            case EROFS:         throw FileReadOnlyError(message, path);
            case ENOMEM:
            case ENOSPC:        throw FileTooLargeError(message, path);
            default:            break;
        }

        throw UnknownFileError(message, path);
    }

}