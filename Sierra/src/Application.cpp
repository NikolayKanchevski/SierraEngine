//
// Created by Nikolay Kanchevski on 21.08.23.
//

#include "Application.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const ApplicationCreateInfo &createInfo)
        : name(createInfo.name), version(createInfo.version), maxFrameRate(createInfo.settings.maxFrameRate)
    {
        Logger::Initialize(name);
        SR_ERROR_IF(createInfo.name.empty(), "Application title must not be empty!");

        // Create objects
        platformContext = PlatformContext::Load({ });
        windowManager = WindowManager::Create({ .platformContext = platformContext });
        renderingContext = RenderingContext::Create({ .name = "Application Context", .graphicsAPI = createInfo.settings.graphicsAPI });
    }

    void Application::Run()
    {
        platformContext->RunApplication({
            .OnStart = [this] {
                OnStart();
            },
            .OnUpdate = [this] {
                if (OnUpdate(frameLimiter.BeginFrame()))
                {
                    return true;
                }

                frameLimiter.ThrottleFrame(maxFrameRate);
                return false;
            },
            .OnEnd = [this] {
                #if SR_PLATFORM_iOS
                    this->~Application(); // Some genious at Apple has decided iOS should do exit(0) internally, which does not call destructors, so manual memory freeing is mandatory
                #else

                #endif
            }
        });
    }

    /* --- POLLING METHODS --- */

    TimeStep Application::FrameLimiter::BeginFrame()
    {
        frameStartTime = TimePoint::Now();
        return deltaTime;
    }

    void Application::FrameLimiter::ThrottleFrame(const uint32 targetFrameRate)
    {
        // Calculate delta time
        deltaTime = frameStartTime - lastFrameStartTime;

        // Target frame rate equal to 0 means no throttling should be applied
        if (targetFrameRate == 0) return;

        // Calculate how long a frame is supposed to last
        const TimeStep REQUIRED_FRAME_TIME = TimeStep(1'000.0f / static_cast<float64>(targetFrameRate));

        // If frame was faster that it should be
        if (deltaTime < REQUIRED_FRAME_TIME)
        {
            // Sleep until frame time has been reached
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<long long>((REQUIRED_FRAME_TIME - deltaTime).GetDurationInMilliseconds())));

            // Recalculate delta time to account for the sleep time as well
            deltaTime = TimeStep(TimePoint::Now() - lastFrameStartTime);
        }

        // Assign current start frame time to last
        lastFrameStartTime = frameStartTime;
    }

    /* --- PROTECTED METHODS --- */

    // NOTE: When querying application folders on Apple platforms, we do not create directories ourselves,
    //       as OS automatically makes them for the application, and restricts access to others (on iOS).

    const std::filesystem::path& Application::GetApplicationDataDirectoryPath()
    {
        static auto path = File::GetApplicationDataDirectoryPath()
           #if !SR_PLATFORM_APPLE
               / name
           #elif SR_PLATFORM_macOS
               / "Contents" / "Resources"  // NOTE: iOS restricts us to putting application data in the bundle directly, so we do the same for macOS (but there we can use the Contents/Resources folder)
           #endif
        ;
        return path;
    }
    const std::filesystem::path& Application::GetApplicationCachesDirectoryPath()
    {
        static auto path = File::GetCachesDirectoryPath()
           #if !SR_PLATFORM_APPLE
               / name
           #endif
        ;
        return path;
    }
    const std::filesystem::path& Application::GetApplicationTemporaryDirectoryPath()
    {
        static auto path = File::GetTemporaryDirectoryPath()
           #if !SR_PLATFORM_APPLE
               / name
           #endif
        ;
        return path;
    }

}
