//
// Created by Nikolay Kanchevski on 10.16.2024.
//

#include "Win32FileErrorHandler.h"

#include "../FileErrors.h"
#include "../PathErrors.h"

namespace Sierra
{

    void HandleWin32FileError(const UINT errorCode, const std::string_view message, const std::filesystem::path& path)
    {
        if (errorCode == ERROR_SUCCESS) return;

        switch (errorCode)
        {
            case ERROR_UNIDENTIFIED_ERROR:         break;
            case ERROR_BAD_PATHNAME:               throw PathInvalidError(message, path);
            case ERROR_FILE_NOT_FOUND:             throw PathMissingError(message, path);
            case ERROR_FILE_EXISTS:                throw PathAlreadyExistsError(message, path);
            case ERROR_ACCESS_DENIED:              throw FileAccessDeniedError(message, path);
            case ERROR_SHARING_VIOLATION:          throw FileLockedError(message, path);
            case ERROR_FILE_CORRUPT:               throw FileCorruptedError(message, path);
            case ERROR_FILE_READ_ONLY:             throw FileReadOnlyError(message, path);
            case ERROR_FILE_TOO_LARGE:             throw FileTooLargeError(message, path);
            default:                               break;
        }

        throw UnknownFileError(message, path);
    }

}