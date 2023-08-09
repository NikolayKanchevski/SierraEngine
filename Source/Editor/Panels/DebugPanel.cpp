//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "DebugPanel.h"

#include "../GUI.h"
#include "../../Engine/Classes/Time.h"

namespace Sierra::Editor
{
    using namespace Engine;

    /* --- POLLING METHODS --- */

    void DebugPanel::DrawUI(const DetailedDebugPanelInput &input)
    {
        if (GUI::BeginWindow("Detailed Stats"))
        {
            GUI::Text("GPU Draw Time: %fms", input.frameDrawTime);

            static constexpr uint32 SAMPLE_COUNT = 200;
            static constexpr uint32 REFRESH_RATE = 60;

            static uint32 currentSampleIndex = 0;

            static float drawTimeSamples[SAMPLE_COUNT] = {};
            static float frameTimeSamples[SAMPLE_COUNT] = {};

            static double refreshTime = Time::GetUpTime();
            while (refreshTime < Time::GetUpTime())
            {
                drawTimeSamples[currentSampleIndex] = input.frameDrawTime;
                frameTimeSamples[currentSampleIndex] = Time::GetFPS();

                currentSampleIndex = (currentSampleIndex + 1) % SAMPLE_COUNT;
                refreshTime += 1.0f / REFRESH_RATE;
            }

            float averageDrawTime = 0.0f;
            for (int32 i = SAMPLE_COUNT; i--;) averageDrawTime += drawTimeSamples[i];
            averageDrawTime /= (float) SAMPLE_COUNT;

            float averageFrameTime = 0.0f;
            for (int32 i = SAMPLE_COUNT; i--;) averageFrameTime += frameTimeSamples[i];
            averageFrameTime /= (float) SAMPLE_COUNT;

            char drawTimeOverlay[32];
            snprintf(drawTimeOverlay, 32, "Average: %f", averageDrawTime);
            GUI::SetNextItemWidthToFit();
            GUI::LineDiagram("Lines", drawTimeSamples, SAMPLE_COUNT, currentSampleIndex, drawTimeOverlay, 0.0f, 100.0f, { 0.0f, 80.0f });

            GUI::InsertSeparator();

            GUI::Text("CPU Frame Time: %i FPS", Time::GetFPS());

            char frameTimeOverlay[32];
            snprintf(frameTimeOverlay, 32, "Average: %f", averageFrameTime);
            GUI::SetNextItemWidthToFit();
            GUI::LineDiagram("Lines", frameTimeSamples, SAMPLE_COUNT, currentSampleIndex, frameTimeOverlay, 0.0f, 1000.0f, { 0.0f, 80.0f });
        }
        GUI::EndWindow();
    }
}
