//
// Created by Nikolay Kanchevski on 8.06.24.
//

#pragma once

#include "Time.h"

namespace Sierra
{

    struct FrameLimiterCreateInfo
    {
        uint32 maxFrameRate = 60;
    };

    class SIERRA_API FrameLimiter
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FrameLimiter(const FrameLimiterCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        Sierra::TimeStep BeginFrame();
        void EndFrame();

        /* --- SETTER METHODS --- */
        void SetMaxFrameRate(uint32 frameRate);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetMaxFrameRate() const { return maxFrameRate; }
        [[nodiscard]] Sierra::TimePoint GetFrameStartTime() const { return frameStartTime; }

        /* --- OPERATORS --- */
        FrameLimiter(const FrameLimiter&) = delete;
        FrameLimiter &operator=(const FrameLimiter&) = delete;

        /* --- DESTRUCTOR --- */
        ~FrameLimiter() = default;

    private:
        uint32 maxFrameRate = 0;
        Sierra::TimePoint frameStartTime;
        Sierra::TimePoint lastFrameStartTime;

    };

}
