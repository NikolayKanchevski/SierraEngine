//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "RendererViewportPanel.h"

#include "../GUI.h"
#include "../Editor.h"
#include "../../Engine/Classes/Math.h"
#include "../../Engine/Classes/Input.h"
#include "../../Engine/Classes/Entity.h"
#include "../../Engine/Components/Camera.h"
#include "../../Engine/Components/Transform.h"

namespace Sierra::Editor
{
    using namespace Engine;

    /* --- POLLING METHODS --- */

    RendererViewportPanel::RendererViewportPanelOutput RendererViewportPanel::DrawUI(const RendererViewportPanelInput &input)
    {

        // Create empty output
        RendererViewportPanelOutput output{};

        // Create scene view tab
        ImDrawList *sceneDrawList;
        if (GUI::BeginWindow("Scene View"))
        {
            sceneDrawList = GUI::GetCurrentWindow()->DrawList;

            // Set output data
            output.xSceneViewPosition = GUI::GetCurrentWindow()->WorkRect.GetTL().x;
            output.ySceneViewPosition = GUI::GetCurrentWindow()->WorkRect.GetTL().y;
            output.sceneViewWidth = GUI::GetRemainingHorizontalSpace();
            output.sceneViewHeight = GUI::GetRemainingVerticalSpace();

            // Flip renderer image
            if (input.renderedTextureDescriptorSet != VK_NULL_HANDLE) GUI::Texture(static_cast<ImTextureID>(input.renderedTextureDescriptorSet), GUI::GetRemainingWindowSpace());
            GUI::EndWindow();
        }

        if (!input.imGuiInstance->HasImGuizmoLayer()) return output;

        // Prepare ImGuizmo
        ImGuizmo::Enable(true);
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetRect(output.xSceneViewPosition, output.ySceneViewPosition, output.sceneViewWidth, output.sceneViewHeight);
        ImGuizmo::SetDrawlist(sceneDrawList);

        // Set gizmo mode
        static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
        if (Input::GetKeyPressed(Key::C)) operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE;
        if (Input::GetKeyPressed(Key::R)) operation = ImGuizmo::SCALE;
        if (Input::GetKeyPressed(Key::E)) operation = ImGuizmo::ROTATE;
        if (Input::GetKeyPressed(Key::W)) operation = ImGuizmo::TRANSLATE;

        // Set the renderer's context
        ImGuizmo::SetImGuiContext(input.imGuiInstance->GetImGuiContext());

        // Convert camera's view matrix to array data (for now use the first spawned camera)
        Camera &camera = World::GetComponent<Camera>(World::GetAllComponentsOfType<Camera>()[0]);
        Matrix4x4 viewMatrix = camera.GetViewMatrix();
        Matrix4x4 projectionMatrix = camera.GetProjectionMatrix();

        // Show cube view
        ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), 10.0f, { output.xSceneViewPosition + output.sceneViewWidth - output.sceneViewWidth / 11.25f, output.ySceneViewPosition }, { output.sceneViewWidth / 11.25f, output.sceneViewWidth / 11.25f }, 0x0000000);

        // TODO: Get back to this after reimplementing the inheritance logic
//        // Decompose modified view matrix to get new yaw and pitch
//        Matrix4x4 inverted = glm::inverse(viewMatrix);
//        Vector3 direction = -Vector3(inverted[2]);
//
//        // Update the local yaw and pitch
//        float newYaw = camera.GetYaw();
//        float newPitch = camera.GetPitch();
//
//        float deltaYaw = glm::degrees(glm::atan(direction.z, direction.x)) - newYaw;
//        newYaw += deltaYaw;
//
//        float deltaPitch = glm::degrees(glm::asin(-direction.y)) + newPitch;
//        newPitch -= deltaPitch;
//
//        newPitch = Math::Clamp(newPitch, -85.0f, 85.0f);
//
//        // Apply yaw and pitch changes to the camera
//        Transform &cameraTransform = camera.GetComponent<Transform>();
//        cameraTransform.SetRotation(newYaw, newPitch, NO_CHANGE);

        // If an object is selected
        if (!Editor::GetSelectedEntity().IsNull())
        {
            // Scissor gizmos so they don't go beyond the window
            sceneDrawList->PushClipRect({ output.xSceneViewPosition, output.ySceneViewPosition }, { output.xSceneViewPosition + output.sceneViewWidth, output.ySceneViewPosition + output.sceneViewHeight });

            Transform &transform = Editor::GetSelectedEntity().GetComponent<Transform>();

            // Convert object's transform into an array
            Matrix4x4 modelMatrix = Math::CreateModelMatrix(transform.GetWorldPosition(), transform.GetRotation(), transform.GetScale());

            // Set snapping
            float snapDeterminant = Input::GetKeyHeld(Key::LEFT_SHIFT) ? (operation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f) : 0.0f;
            float snapping[3] = { snapDeterminant, snapDeterminant, snapDeterminant };

            // Show gizmos
            if (ImGuizmo::IsUsingViewManipulate()) return output;
            ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), operation, ImGuizmo::MODE::LOCAL, glm::value_ptr(modelMatrix), nullptr, snapping);

            // Retrieve and apply gizmo's changes
            if (ImGuizmo::IsUsing())
            {
                // Decompose model matrix
                Vector3 translation, rotation, scale;
                if (!Math::DecomposeModelMatrix(modelMatrix, translation, rotation, scale)) return output;

                // Swap X and Y rotation to suit the engine's axis
                float y = rotation.y;
                rotation.y = rotation.x;
                rotation.x = y;

                // Apply position, rotation, and scale changes whilst inverting Y position, as it has already been inverted once to suit Vulkan's -Y needs
                transform.SetWorldPosition(translation);
                transform.SetRotation(rotation);
                transform.SetScale(scale);
            }
        }

        return output;
    }
}