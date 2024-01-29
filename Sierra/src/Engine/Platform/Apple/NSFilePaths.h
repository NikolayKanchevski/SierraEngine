//
// Created by Nikolay Kanchevski on 2.01.24.
//

#pragma once

#if !SR_PLATFORM_APPLE
    #error "Including the macOSContext.h file is only allowed in Apple builds!"
#endif

namespace Sierra
{

    class SIERRA_API NSFilePaths final
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] static std::filesystem::path GetApplicationDirectoryPath();
        [[nodiscard]] static std::filesystem::path GetUserDirectoryPath();
        [[nodiscard]] static std::filesystem::path GetCachesDirectoryPath();
        [[nodiscard]] static std::filesystem::path GetTemporaryDirectoryPath();

    };

}
