//
// Created by Nikolay Kanchevski on 7.11.24.
//

#include "EditorWizard.h"

namespace SierraEngine
{

    namespace
    {
        constexpr ImGuiWindowFlags MODAL_WINDOW_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
    }

    /* --- POLLING METHODS --- */

    bool EditorWizard::BeginWizard(const std::string_view title, bool& open) const noexcept
    {
        const ImGuiIO& io = ImGui::GetIO();
        const Vector2 windowSize = { io.DisplaySize.x / 2.75f, io.DisplaySize.y / 1.25f };

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos({ (io.DisplaySize.x - windowSize.x) / 2.0f, (io.DisplaySize.y - windowSize.y) / 2.0f });

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImGui::GetStyle().CellPadding * 2);
        if (open)
        {
            ImGui::OpenPopup(title.data());
        }

        if (ImGui::BeginPopupModal(title.data(), &open, MODAL_WINDOW_FLAGS))
        {
            return true;
        }

        ImGui::PopStyleVar();
        return false;
    }

    void EditorWizard::EndWizard() const noexcept
    {
        ImGui::PopStyleVar();
        ImGui::End();
    }

}