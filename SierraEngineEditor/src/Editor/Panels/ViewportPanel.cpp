//
// Created by Nikolay Kanchevski on 24.10.24.
//

#include "ViewportPanel.h"

namespace SierraEngine
{

    void ViewportPanel::Draw(const ViewportPanelDrawInfo& drawInfo)
    {
        if (ImGui::Begin(drawInfo.title.data(), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNav))
        {
            const uint32 width = static_cast<uint32>(ImGui::GetContentRegionAvail().x);
            const uint32 height = static_cast<uint32>(ImGui::GetContentRegionAvail().y);
            if (width == 0 || height == 0)
            {
                ImGui::End();
                return;
            }

            Viewport& viewport = drawInfo.viewport;
            viewport.SetWidth(width);
            viewport.SetHeight(height);

            ImGui::Image(ImTextureRef(viewport.GetImageID()), { static_cast<float32>(width), static_cast<float32>(height) });
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                constexpr float32 CAMERA_MOVE_SPEED = 0.25f;
                constexpr float32 CAMERA_LOOK_SPEED = 0.20f;

                Transform& transform = viewport.GetTransform();
                transform.SetRotation(transform.GetRotation() + Vector3(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y, 0) * CAMERA_LOOK_SPEED);

                if (ImGui::IsKeyDown(ImGuiKey_W)) transform.SetPosition(transform.GetPosition() + transform.GetForwardDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_S)) transform.SetPosition(transform.GetPosition() + transform.GetBackwardDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_A)) transform.SetPosition(transform.GetPosition() + transform.GetLeftDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_D)) transform.SetPosition(transform.GetPosition() + transform.GetRightDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_E) || ImGui::IsKeyDown(ImGuiKey_Space))    transform.SetPosition(transform.GetPosition() + transform.GetUpDirection() * CAMERA_MOVE_SPEED);
                if (ImGui::IsKeyDown(ImGuiKey_Q) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) transform.SetPosition(transform.GetPosition() + transform.GetDownDirection() * CAMERA_MOVE_SPEED);
            }
        }
        ImGui::End();
    }

}