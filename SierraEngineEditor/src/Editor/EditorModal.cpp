//
// Created by Nikolay Kanchevski on 7.11.24.
//

#include "EditorModal.h"

namespace SierraEngine
{

    namespace
    {
        constexpr ImGuiWindowFlags MODAL_WINDOW_FLAGS = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
    }

    /* --- POLLING METHODS --- */

    bool EditorModal::BeginWizard(const std::string_view title) const noexcept
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
            return false;

        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowSize({ io.DisplaySize.x / 2.75f, 0 });

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos({ viewport->WorkPos.x + viewport->WorkSize.x * 0.5f, viewport->WorkPos.y + viewport->WorkSize.y * 0.5f }, ImGuiCond_Always, { 0.5f, 0.5f });

        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImGui::GetStyle().CellPadding * 2);
        ImGui::OpenPopup(title.data());

        if (bool open = true; ImGui::BeginPopupModal(title.data(), &open, MODAL_WINDOW_FLAGS))
        {
            return true;
        }

        ImGui::PopStyleVar();
        return false;
    }

    void EditorModal::EndWizard() const noexcept
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        const Vector2 windowSize = ImGui::GetWindowSize();

        const ImGuiIO& io = ImGui::GetIO();
        ImGui::SetWindowPos(window, { (io.DisplaySize.x - windowSize.x) / 2.0f, (io.DisplaySize.y - windowSize.y) / 2.0f });

        ImGui::PopStyleVar();
        ImGui::EndPopup();
    }

}