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
        explicit FrameLimiter(const FrameLimiterCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        Sierra::TimeStep BeginFrame() noexcept;
        void EndFrame() noexcept;

        /* --- SETTER METHODS --- */
        void SetMaxFrameRate(uint32 frameRate) noexcept;

        /* --- GETTER METHODS --- */
        [[nodiscard]] uint32 GetMaxFrameRate() const noexcept { return maxFrameRate; }
        [[nodiscard]] Sierra::TimePoint GetFrameStartTime() const noexcept { return frameStartTime; }

        /* --- COPY SEMANTICS --- */
        FrameLimiter(const FrameLimiter&) = delete;
        FrameLimiter& operator=(const FrameLimiter&) = delete;

        /* --- MOVE SEMANTICS --- */
        FrameLimiter(FrameLimiter&&) noexcept = default;
        FrameLimiter& operator=(FrameLimiter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~FrameLimiter() noexcept = default;

    private:
        uint32 maxFrameRate = 0;
        Sierra::TimePoint frameStartTime;
        Sierra::TimePoint lastFrameStartTime;

    };

}
