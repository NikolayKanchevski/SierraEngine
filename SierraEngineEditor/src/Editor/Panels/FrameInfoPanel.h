//
// Created by Nikolay Kanchevski on 11.19.26.
//

#pragma once

namespace SierraEngine
{

    struct FrameInfoPanelDrawInfo
    {
        Sierra::TimeStep frameTime = { };
        std::optional<Sierra::TimeStep> drawTime = std::nullopt; // None if the GPU time could not be measured
    };

    class FrameInfoPanel
    {
    public:
        /* --- CONSTRUCTORS --- */
        FrameInfoPanel() noexcept = default;

        /* --- POLLING METHODS --- */
        void Draw(const FrameInfoPanelDrawInfo& drawInfo) noexcept;

        /* --- COPY SEMANTICS --- */
        FrameInfoPanel(const FrameInfoPanel&) = delete;
        FrameInfoPanel& operator=(const FrameInfoPanel&) = delete;

        /* --- MOVE SEMANTICS --- */
        FrameInfoPanel(FrameInfoPanel&&) noexcept = default;
        FrameInfoPanel& operator=(FrameInfoPanel&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~FrameInfoPanel() noexcept = default;

    private:
        // Samples are taken at a fixed rate, so that graphs scroll at the same speed regardless of frame rate (here, covering the last ~2 seconds)
        constexpr static size SAMPLE_COUNT = 128;
        constexpr static float64 SAMPLE_RATE = 60.0;

        std::array<float32, SAMPLE_COUNT> frameTimeSamples = { };
        std::array<float32, SAMPLE_COUNT> drawTimeSamples = { };

        size currentSampleIndex = 0;
        float64 timeSinceLastSample = 0.0;

    };

}
