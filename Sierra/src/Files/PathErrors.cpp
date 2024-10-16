//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "PathErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    PathError::PathError(const std::string_view message, const std::string_view error) noexcept
        : std::runtime_error(SR_FORMAT("{0}! Error: {1}.", message, error))
    {

    }

    /* --- CONSTRUCTORS --- */

    PathInvalidError::PathInvalidError(const std::string_view message, const std::filesystem::path& path) noexcept
        : PathError(message, SR_FORMAT("Invalid path [{0}]", path.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    PathMissingError::PathMissingError(const std::string_view message, const std::filesystem::path& path) noexcept
        : PathError(message, SR_FORMAT("No such path [{0}]", path.string()))
    {

    }

    /* --- CONSTRUCTORS --- */

    PathAlreadyExistsError::PathAlreadyExistsError(const std::string_view message, const std::filesystem::path& path) noexcept
        : PathError(message, SR_FORMAT("Path [{0}] already exists", path.string()))
    {

    }

}