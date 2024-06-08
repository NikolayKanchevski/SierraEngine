//
// Created by Nikolay Kanchevski on 21.08.23.
//

#include "Application.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const ApplicationCreateInfo &createInfo)
        : name(createInfo.name), version(createInfo.version), maxFrameRate(createInfo.settings.maxFrameRate), fileManager({ })
    {
        #if SR_ENABLE_LOGGING
            Logger::Initialize(name);
        #endif
        SR_ERROR_IF(createInfo.name.empty(), "Application title must not be empty!");

        // Create objects
        platformContext = PlatformContext::Create({ });
        windowManager = WindowManager::Create({ .platformContext = *platformContext });
        renderingContext = RenderingContext::Create({ .name = "Application Rendering Context", .graphicsAPI = createInfo.settings.graphicsAPI });
    }

    /* --- PROTECTED METHODS --- */

    // NOTE: When querying application folders on Apple platforms, we do not create directories ourselves,
    //       as OS automatically makes them for the application, and restricts access to others (on iOS).

    const std::filesystem::path& Application::GetApplicationCachesDirectoryPath() const
    {
        static const std::filesystem::path path = fileManager.GetTemporaryDirectoryPath()
           #if !SR_PLATFORM_APPLE
               / name
           #endif
        ;
        return path;
    }
    const std::filesystem::path& Application::GetApplicationTemporaryDirectoryPath() const
    {
        static const std::filesystem::path path = fileManager.GetTemporaryDirectoryPath()
           #if !SR_PLATFORM_APPLE
               / name
           #endif
        ;
        return path;
    }

}
