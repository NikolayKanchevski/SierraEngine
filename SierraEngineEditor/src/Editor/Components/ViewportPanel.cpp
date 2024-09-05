//
// Created by Nikolay Kanchevski on 24.06.24.
//

#include "ViewportPanel.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void ViewportPanel::Draw(const Sierra::Image& renderedImage)
    {
        if (ImGui::Begin("Rendered Image", nullptr, 0))
        {
            ImGui::Image(10, { ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f });
            ImGui::End();
        }
    }


}