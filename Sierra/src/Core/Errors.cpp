//
// Created by Nikolay Kanchevski on 30.09.24.
//
#include "Errors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    UnsupportedFeatureError::UnsupportedFeatureError(const std::string_view message) noexcept
        : std::logic_error(SR_FORMAT("{0}! Error: Feature unsupported.", message))
    {

    }

    /* --- CONSTRUCTORS --- */

    UnexpectedTypeError::UnexpectedTypeError(const std::string_view message) noexcept
        : std::runtime_error(SR_FORMAT("{0}! Error: Argument is of unexpected type.", message))
    {

    }

    /* --- CONSTRUCTORS --- */

    InvalidValueError::InvalidValueError(const std::string_view message) noexcept
            : std::runtime_error(SR_FORMAT("{0}! Error: Argument value is invalid.", message))
    {

    }

    /* --- CONSTRUCTORS --- */

    InvalidConfigurationError::InvalidConfigurationError(const std::string_view message) noexcept
        : std::runtime_error(SR_FORMAT("{0}! Error: Invalid configuration specified.", message))
    {

    }

    /* --- CONSTRUCTORS --- */

    InvalidOperationError::InvalidOperationError(const std::string_view message) noexcept
        : std::runtime_error(SR_FORMAT("{0}! Error: Invalid operation performed.", message))
    {

    }

    /* --- CONSTRUCTORS --- */

    UnexpectedSizeError::UnexpectedSizeError(const std::string_view message, const size actualSize, const size expectedSize) noexcept
        : std::runtime_error(SR_FORMAT("{0}! Error: Argument size [{1}] does not match expected size of [{2}].", message, actualSize, expectedSize))
    {

    }

}