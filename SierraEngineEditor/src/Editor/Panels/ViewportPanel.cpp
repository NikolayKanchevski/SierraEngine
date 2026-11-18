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

            ImGui::Image(ImTextureRef(viewport.GetImageID().GetValue()), { static_cast<float32>(width), static_cast<float32>(height) });
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Right))
            {
                constexpr float32 CAMERA_MOVE_SPEED = 15.0f; // Units per second
                constexpr float32 CAMERA_LOOK_SPEED = 0.20f; // Degrees per pixel moved, which (unlike movement) must not be scaled by time, as the mouse delta already is this frame's movement

                Transform& transform = viewport.GetTransform();
                transform.SetRotation(transform.GetRotation() + Vector3(ImGui::GetIO().MouseDelta.x, -ImGui::GetIO().MouseDelta.y, 0) * CAMERA_LOOK_SPEED);

                const float32 moveDistance = CAMERA_MOVE_SPEED * drawInfo.timeStep;
                if (ImGui::IsKeyDown(ImGuiKey_W)) transform.SetPosition(transform.GetPosition() + transform.GetForwardDirection() * moveDistance);
                if (ImGui::IsKeyDown(ImGuiKey_S)) transform.SetPosition(transform.GetPosition() + transform.GetBackwardDirection() * moveDistance);
                if (ImGui::IsKeyDown(ImGuiKey_A)) transform.SetPosition(transform.GetPosition() + transform.GetLeftDirection() * moveDistance);
                if (ImGui::IsKeyDown(ImGuiKey_D)) transform.SetPosition(transform.GetPosition() + transform.GetRightDirection() * moveDistance);
                if (ImGui::IsKeyDown(ImGuiKey_E) || ImGui::IsKeyDown(ImGuiKey_Space))    transform.SetPosition(transform.GetPosition() + transform.GetUpDirection() * moveDistance);
                if (ImGui::IsKeyDown(ImGuiKey_Q) || ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) transform.SetPosition(transform.GetPosition() + transform.GetDownDirection() * moveDistance);
            }
        }
        ImGui::End();
    }

}