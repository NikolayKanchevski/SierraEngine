//
// Created by Nikolay Kanchevski on 11.19.26.
//

#include "FrameInfoPanel.h"

#include <numeric>

namespace SierraEngine
{

    namespace
    {
        constexpr float32 GRAPH_HEIGHT = 80.0f;

        [[nodiscard]] float32 GetAverage(const std::span<const float32> samples) noexcept
        {
            return std::accumulate(samples.begin(), samples.end(), 0.0f) / static_cast<float32>(samples.size());
        }

        void DrawGraph(const char* label, const std::span<const float32> samples, const size oldestSampleIndex, const bool drawFramesPerSecond = false)
        {
            const float32 highestSample = *std::ranges::max_element(samples);
            const float32 average = GetAverage(samples);


            const uint32 framesPerSecond = average > 0.0f ? static_cast<uint32>(1000.0f / average) : 0U;
            const std::string overlay = SR_FORMAT("Average: {0:.3f} ms{1}", average, drawFramesPerSecond ? SR_FORMAT(" | {0} FPS", framesPerSecond) : "");

            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
            ImGui::PlotLines(label, samples.data(), static_cast<int>(samples.size()), static_cast<int>(oldestSampleIndex), overlay.c_str(), 0.0f, glm::max(highestSample * 1.25f, 0.001f), { 0.0f, GRAPH_HEIGHT });
        }
    }

    /* --- POLLING METHODS --- */

    void FrameInfoPanel::Draw(const FrameInfoPanelDrawInfo& drawInfo) noexcept
    {
        const float32 frameMilliseconds = static_cast<float32>(drawInfo.frameTime.GetDuration(Sierra::TimeStepUnit::Milliseconds));
        const float32 drawMilliseconds = drawInfo.drawTime.has_value() ? static_cast<float32>(drawInfo.drawTime->GetDuration(Sierra::TimeStepUnit::Milliseconds)) : 0.0f;

        // Record a sample for every interval elapsed since the last one (at most a full history's worth, so a long hitch does not stall here)
        timeSinceLastSample += drawInfo.frameTime.GetDuration(Sierra::TimeStepUnit::Seconds);
        for (size i = 0; timeSinceLastSample >= 1.0 / SAMPLE_RATE && i < SAMPLE_COUNT; i++)
        {
            frameTimeSamples[currentSampleIndex] = frameMilliseconds;
            drawTimeSamples[currentSampleIndex] = drawMilliseconds;

            currentSampleIndex = (currentSampleIndex + 1) % SAMPLE_COUNT;
            timeSinceLastSample -= 1.0 / SAMPLE_RATE;
        }
        timeSinceLastSample = std::fmod(timeSinceLastSample, 1.0 / SAMPLE_RATE);

        if (ImGui::Begin("Frame Info", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNav))
        {
            const uint32 framesPerSecond = frameMilliseconds > 0.0f ? static_cast<uint32>(1000.0f / frameMilliseconds) : 0U;
            ImGui::Text("CPU Frame Time: %.3f ms (%u FPS)", frameMilliseconds, framesPerSecond);
            DrawGraph("##FrameTimeGraph", frameTimeSamples, currentSampleIndex, true);

            ImGui::Separator();

            if (drawInfo.drawTime.has_value())
            {
                ImGui::Text("GPU Draw Time: %.3f ms", drawMilliseconds);
                DrawGraph("##DrawTimeGraph", drawTimeSamples, currentSampleIndex);
            }
            else
            {
                ImGui::TextUnformatted("GPU Draw Time: Unavailable");
            }
        }
        ImGui::End();
    }

}
