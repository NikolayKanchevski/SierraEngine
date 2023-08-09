//
// Created by Nikolay Kanchevski on 30.07.23.
//

#pragma once

namespace Sierra::Editor::DebugPanel
{
    struct DetailedDebugPanelInput
    {
        float frameDrawTime = 0.0f;
    };

    /* --- POLLING METHODS --- */
    void DrawUI(const DetailedDebugPanelInput &input);
}
