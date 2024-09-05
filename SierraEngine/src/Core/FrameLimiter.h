//
// Created by Nikolay Kanchevski on 8.06.24.
//

#pragma once

namespace SierraEngine
{

    struct FrameLimiterCreateInfo
    {
        uint32 maxFrameRate = 60;
    };

    class SIERRA_ENGINE_API FrameLimiter final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit FrameLimiter(const FrameLimiterCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        Sierra::TimeStep BeginFrame();
        void EndFrame();

        /* --- SETTER METHODS --- */
        void SetMaxFrameRate(uint32 frameRate);

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetMaxFrameRate() const { return maxFrameRate; }
        [[nodiscard]] Sierra::TimePoint GetFrameStartTime() const { return frameStartTime; }

        /* --- COPY SEMANTICS --- */
        FrameLimiter(const FrameLimiter&) = delete;
        FrameLimiter& operator=(const FrameLimiter&) = delete;

        /* --- MOVE SEMANTICS --- */
        FrameLimiter(FrameLimiter&&) = default;
        FrameLimiter& operator=(FrameLimiter&&) = default;

        /* --- DESTRUCTOR --- */
        ~FrameLimiter() = default;

    private:
        uint32 maxFrameRate = 0;
        Sierra::TimePoint frameStartTime;
        Sierra::TimePoint lastFrameStartTime;

    };

}
