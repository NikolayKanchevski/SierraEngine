//
// Created by Nikolay Kanchevski on 10.17.24.
//

#pragma once

namespace Sierra
{
    SIERRA_API void HandleUnixFileError(int errorCode, std::string_view message, const std::filesystem::path& path);
}