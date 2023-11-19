//
// Created by Nikolay Kanchevski on 21.08.23.
//

#include "Application.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const ApplicationCreateInfo &createInfo)
        : name(createInfo.name), settings(createInfo.settings), version(createInfo.version)
    {
        Logger::Initialize(name);
        SR_ERROR_IF(createInfo.name.empty(), "Application title must not be empty!");

        // Create platform objects
        platformInstance = PlatformInstance::Load({ });
        windowManager = WindowManager::Create({ .platformInstance = platformInstance });

        // Create rendering objects
        renderingContext = RenderingContext::Create({ .graphicsAPI = createInfo.settings.graphicsAPI });
    }

    void Application::Run()
    {
        platformInstance->RunApplication({
            .OnStart = [this] {
                OnStart();
            },
            .OnUpdate = [this] {
                if (OnUpdate(frameLimiter.BeginFrame()))
                {
                    return true;
                }

                frameLimiter.ThrottleFrame(settings.maxFrameRate);
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

    /* --- DESTRUCTOR --- */

    Application::~Application()
    {

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

}
