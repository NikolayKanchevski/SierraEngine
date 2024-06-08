//
// Created by Nikolay Kanchevski on 8.06.24.
//

#include "FrameLimiter.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    FrameLimiter::FrameLimiter(const FrameLimiterCreateInfo &createInfo)
        : maxFrameRate(createInfo.maxFrameRate)
    {

    }

    /* --- POLLING METHODS --- */

    TimeStep FrameLimiter::BeginFrame()
    {
        frameStartTime = TimePoint::Now();
        return frameStartTime - lastFrameStartTime;
    }

    void FrameLimiter::EndFrame()
    {
        // Enforce frame limit if set
        if (maxFrameRate != 0)
        {
            // If frame time has been less than limit, sleep until it is time for next frame
            const TimeStep expectedFrameTime = TimeStep(1'000.0f / static_cast<float64>(maxFrameRate));
            if (const TimeStep frameTime = TimePoint::Now() - frameStartTime; frameTime < expectedFrameTime)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<llong>((expectedFrameTime - frameTime).GetDurationInMilliseconds())));
            }
        }

        lastFrameStartTime = frameStartTime;
    }

    /* --- SETTER METHODS --- */

    void FrameLimiter::SetMaxFrameRate(const uint32 frameRate)
    {
        maxFrameRate = frameRate;
    }

}