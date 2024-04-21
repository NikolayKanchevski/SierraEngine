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
        windowManager = WindowManager::Create({ .platformContext = platformContext });
        renderingContext = RenderingContext::Create({ .name = "Application Context", .graphicsAPI = createInfo.settings.graphicsAPI });
    }

    void Application::Run()
    {
        platformContext->RunApplication({
            .OnStart = [this] {
                Start();
            },
            .OnUpdate = [this] {
                const TimePoint frameStartTime = TimePoint::Now();
                if (Update(frameStartTime - lastFrameStartTime))
                {
                    return true;
                }

                // Enforce frame limit if set
                if (maxFrameRate != 0)
                {
                    const TimeStep expectedFrameTime = TimeStep(1'000.0f / static_cast<float64>(maxFrameRate));
                    const TimeStep frameTime = TimePoint::Now() - frameStartTime;

                    // If frame time has been less than limit, sleep until it is time for next frame
                    if (frameTime < expectedFrameTime)
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<llong>((expectedFrameTime - frameTime).GetDurationInMilliseconds())));
                    }
                }

                lastFrameStartTime = frameStartTime;
                return false;
            },
            .OnEnd = [this] {
                #if SR_PLATFORM_iOS
                    this->~Application(); // Some genius at Apple decided application should do exit() internally, which does not call destructors, so manual memory freeing is mandatory
                #else

                #endif
            }
        });
    }

    /* --- PROTECTED METHODS --- */

    // NOTE: When querying application folders on Apple platforms, we do not create directories ourselves,
    //       as OS automatically makes them for the application, and restricts access to others (on iOS).

    const std::filesystem::path& Application::GetApplicationCachesDirectoryPath()
    {
        static auto path = fileManager.GetTemporaryDirectoryPath()
           #if !SR_PLATFORM_APPLE
               / name
           #endif
        ;
        return path;
    }
    const std::filesystem::path& Application::GetApplicationTemporaryDirectoryPath()
    {
        static auto path = fileManager.GetTemporaryDirectoryPath()
           #if !SR_PLATFORM_APPLE
               / name
           #endif
        ;
        return path;
    }

}
