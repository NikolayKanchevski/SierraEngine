//
// Created by Nikolay Kanchevski on 4.10.24.
//
#include "DeviceErrors.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    DeviceError::DeviceError(const std::string_view message, const std::string_view error) noexcept
        : std::runtime_error(SR_FORMAT("{0}! Error: {1}.", message, error))
    {

    }

    /* --- CONSTRUCTORS --- */

    UnknownDeviceError::UnknownDeviceError(const std::string_view message) noexcept
        : DeviceError(message, SR_FORMAT("Unknown error"))
    {

    }
    /* --- CONSTRUCTORS --- */

    DeviceOutOfMemory::DeviceOutOfMemory(const std::string_view message) noexcept
        : DeviceError(message, SR_FORMAT("Insufficient memory"))
    {

    }

}