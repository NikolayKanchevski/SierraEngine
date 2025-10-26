//
// Created by Nikolay Kanchevski on 16.10.25.
//

#include "PlatformContext.h"

namespace Sierra
{

    /* --- POLLING METHODS --- */

    std::optional<std::filesystem::path> PlatformContext::OpenSingleFileSelectDialog(const FileSelectDialogOpenInfo& openInfo) const noexcept
    {
        std::vector<std::filesystem::path> filePaths = OpenFileSelectDialog(openInfo);
        if (filePaths.empty()) return std::nullopt;

        return filePaths.front();
    }

}