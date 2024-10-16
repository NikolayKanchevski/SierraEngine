//
// Created by Nikolay Kanchevski on 10.16.2024.
//

#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace Sierra
{
    SIERRA_API void HandleWin32FileError(UINT errorCode, std::string_view message, const std::filesystem::path& path);
}