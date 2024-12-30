//
// Created by Nikolay Kanchevski on 7.11.24.
//

#include "EditorWizard.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void EditorWizard::DrawShadow()
    {
        constexpr float32 DIMMING_FACTOR = 0.5f;
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, DIMMING_FACTOR));

        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGui::Begin("##WizardShadow", nullptr, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
        ImGui::End();

        ImGui::PopStyleColor();
    }

}