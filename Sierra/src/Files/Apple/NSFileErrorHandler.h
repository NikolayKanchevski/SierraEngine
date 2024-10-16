//
// Created by Nikolay Kanchevski on 23.09.24.
//

#pragma once

#if !SR_PLATFORM_APPLE
    #error "Including the NSFileErrorHandler.h file is only allowed in Apple builds!"
#endif

#if defined(__OBJC__)
    #include <Foundation/Foundation.h>
#else
    namespace Sierra
    {
        using NSError = void;
    }
#endif

namespace Sierra
{
    SIERRA_API void HandleNSFileError(const NSError* error, std::string_view message, const std::filesystem::path& path);
}