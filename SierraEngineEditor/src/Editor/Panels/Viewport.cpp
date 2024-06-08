//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "Viewport.h"

namespace SierraEngine
{
    /* --- POLLING METHODS --- */

    void Viewport::Draw()
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        // Create viewport
        ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize  | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleVar(2);

        // Draw menu bar
        ImGui::DockSpace(ImGui::GetID("Dockspace"), { 0.0f, 0.0f }, ImGuiDockNodeFlags_PassthruCentralNode);
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Settings"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("I wanna die"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        ImGui::End();
    }
}