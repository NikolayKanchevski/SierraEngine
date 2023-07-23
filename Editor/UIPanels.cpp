//
// Created by Nikolay Kanchevski on 17.02.23.
//

#include "UIPanels.h"

#include "../Editor/GUI.h"
#include "../Engine/Classes/Time.h"
#include "../Engine/Classes/Math.h"
#include "../Engine/Classes/Input.h"
#include "../Engine/Classes/Discord.h"
#include "../Engine/Classes/SystemInformation.h"
#include "../Engine/Components/ComponentsInclude.h"

using namespace Sierra::Rendering;
namespace Sierra::Editor
{
    // ********************* Main Viewport Panel ********************* \\

    void MainViewportPanel::DrawUI()
    {
        // Set up dock space and window flags
        WindowFlags windowFlags = WindowFlags::MENU_BAR | WindowFlags::NO_DOCKING | WindowFlags::NO_BACKGROUND |
                                  WindowFlags::NO_TITLE_BAR | WindowFlags::NO_COLLAPSE | WindowFlags::NO_RESIZE | WindowFlags::NO_MOVE |
                                  WindowFlags::NO_BRING_TO_FRONT_ON_FOCUS | WindowFlags::NO_NAV_FOCUS;

        // Get a pointer to the main viewport of ImGui
        const auto viewport = GUI::GetMainViewport();

        // Set window sizing properties accordingly
        GUI::SetNextWindowPosition({ viewport->WorkPos.x, viewport->WorkPos.y });
        GUI::SetNextWindowSize({ viewport->WorkSize.x, viewport->WorkSize.y });

        // Disable window padding
        GUI::PushStyleVariable(StyleVariable::WINDOW_PADDING, { 0.0f, 0.0f });

        // Create main viewport window
        GUI::BeginWindow("Viewport", windowFlags);

        // Remove the overridden window padding
        GUI::PopStyleVariable();

        // Use dock space
        GUI::CreateDockSpace("ViewportDock");

        // Create menu bar
        if (GUI::BeginMenuBar())
        {
            if (GUI::BeginMenuBarTab("File"))
            {
                GUI::EndMenuBarTab();
            }

            if (GUI::BeginMenuBarTab("Edit"))
            {
                GUI::EndMenuBarTab();
            }

            if (GUI::BeginMenuBarTab("Preferences"))
            {
                GUI::EndMenuBarTab();
            }

            if (GUI::BeginMenuBarTab("I wanna die"))
            {
                GUI::EndMenuBarTab();
            }

            GUI::EndMenuBar();
        }

        // Finalize main viewport window
        GUI::EndWindow();
    }

    using namespace Engine;
    void ListDeeper(Relationship &relationship, const uint iteration)
    {
        TreeFlags treeFlags = TreeFlags::OPEN_ON_ARROW | TreeFlags::FRAME_PADDING | TreeFlags::SPAN_AVAILABLE_WIDTH;

        bool selected = false;
        if (!World::GetSelectedEntity().IsNull() && World::GetSelectedEntity().GetComponent<UUID>() == relationship.GetComponent<UUID>())
        {
            selected = true;
            treeFlags |= TreeFlags::SELECTED;
            GUI::PushStyleColor(StyleColor::HEADER,         { 0.11f, 0.32f, 0.50f, 1.00f });
            GUI::PushStyleColor(StyleColor::HOVERED_HEADER, { 0.10f, 0.26f, 0.45f, 1.00f });
        }

        bool opened = GUI::BeginTree((void*) relationship.GetComponent<UUID>().GetValue(), treeFlags, "%s", relationship.GetComponent<Tag>().tag.c_str());

        if (GUI::IsItemClicked())
        {
            World::SetSelectedEntity(Entity(relationship.GetEnttEntity()));
        }

        if (selected)
        {
            GUI::PopStyleColor(2);
        }

        if (opened)
        {
            for (auto child : relationship.GetEnttChildrenEntities())
            {
                Relationship &childRelationship = World::GetComponent<Relationship>(child);
                ListDeeper(childRelationship, iteration + 1);
            }

            GUI::EndTree();
        }


        if (iteration == 0) GUI::InsertSeparator();
    }

    // ********************* RendererViewport Panel ********************* \\

    RendererViewportPanelOutput RendererViewportPanel::DrawUI(const RendererViewportPanelInput &input)
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

        // Convert camera's view matrix to array data
        Camera &camera = Camera::GetMainCamera();
        Matrix4x4 viewMatrix = camera.GetViewMatrix();
        Matrix4x4 projectionMatrix = camera.GetProjectionMatrix();

        // Show cube view
        ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), 10.0f, { output.xSceneViewPosition + output.sceneViewWidth - output.sceneViewWidth / 11.25f, output.ySceneViewPosition }, { output.sceneViewWidth / 11.25f, output.sceneViewWidth / 11.25f }, 0x0000000);

        // Decompose modified view matrix to get new yaw and pitch
        Matrix4x4 inverted = glm::inverse(viewMatrix);
        Vector3 direction = -Vector3(inverted[2]);

        // Update the local yaw and pitch
        float newYaw = camera.GetYaw();
        float newPitch = camera.GetPitch();

        float deltaYaw = glm::degrees(glm::atan(direction.z, direction.x)) - newYaw;
        newYaw += deltaYaw;

        float deltaPitch = glm::degrees(glm::asin(-direction.y)) + newPitch;
        newPitch -= deltaPitch;

        newPitch = Math::Clamp(newPitch, -85.0f, 85.0f);

        // Apply yaw and pitch changes to the camera
        Transform &cameraTransform = camera.GetComponent<Transform>();
        cameraTransform.SetRotation(newYaw, newPitch, NO_CHANGE);

        // If an object is selected
        if (!World::GetSelectedEntity().IsNull())
        {
            // Scissor gizmos so they don't go beyond the window
            sceneDrawList->PushClipRect({ output.xSceneViewPosition, output.ySceneViewPosition }, { output.xSceneViewPosition + output.sceneViewWidth, output.ySceneViewPosition + output.sceneViewHeight });

            Transform &transform = World::GetSelectedEntity().GetComponent<Transform>();

            // Convert object's transform into an array
            Matrix4x4 modelMatrix = Math::CreateModelMatrix(transform.GetWorldPosition(), transform.GetRotation(), transform.GetScale());

            // Set snapping
            float snapDeterminant = Input::GetKeyHeld(Key::LEFT_SHIFT) ? (operation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f) : 0.0f;
            float snapping[3] = { snapDeterminant, snapDeterminant, snapDeterminant };

            // Show gizmos
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

    // ********************* Hierarchy Panel ********************* \\

    void HierarchyPanel::DrawUI()
    {
        // Create hierarchy tab
        if (GUI::BeginWindow("Hierarchy", WindowFlags::NO_NAV | WindowFlags::SHOW_HORIZONTAL_SCROLLBAR))
        {
            GUI::InsertSeparator();

            // Recursively show all entities in the hierarchy
            for (const auto &entityData : World::GetOriginEntitiesList())
            {
                Relationship &entityRelationship = World::GetComponent<Relationship>(entityData.second);
                if (entityRelationship.GetEnttParentEntity() == entt::null)
                {
                    ListDeeper(entityRelationship, 0);
                }
            }
        }
        GUI::EndWindow();
    }

    // ********************* Properties Panel ********************* \\

    void PropertiesPanel::DrawUI()
    {
        if (GUI::BeginWindow("Properties", WindowFlags::NO_NAV))
        {
            Entity selectedEntity = World::GetSelectedEntity();
            if (!selectedEntity.IsNull())
            {
                GUI::DrawComponent<UUID>(selectedEntity);
                GUI::DrawComponent<Tag>(selectedEntity);
                GUI::DrawComponent<Transform>(selectedEntity);
                GUI::DrawComponent<MeshRenderer>(selectedEntity);
                GUI::DrawComponent<Camera>(selectedEntity);
                GUI::DrawComponent<DirectionalLight>(selectedEntity);
                GUI::DrawComponent<PointLight>(selectedEntity);
            }
        }
        GUI::EndWindow();
    }

    // ********************* Debug Panel ********************* \\

    void DebugPanel::DrawUI(const DebugPanelInput &input)
    {
        // Create debug tab
        if (GUI::BeginWindow("Debug Information", WindowFlags::NO_NAV))
        {
            GUI::InsertSeparator();
            GUI::Text("CPU Frame Time: %i FPS", Time::GetFPS());
            GUI::Text("GPU Draw Time: %f ms", input.frameDrawTime);
            GUI::InsertSeparator();
            GUI::Text("Total meshes being drawn: %i", Mesh::GetTotalMeshCount());
            GUI::Text("Total vertices in scene: %u", Mesh::GetTotalVertexCount() + (GUI::GetDrawData() != nullptr ? GUI::GetDrawData()->TotalVtxCount : 0));
        }
        GUI::EndWindow();
    }

    // ********************* Detailed Debug Panel ********************* \\

    void DetailedDebugPanel::DrawUI(const DebugPanelInput &input)
    {
        if (GUI::BeginWindow("Detailed Stats"))
        {
            GUI::Text("GPU Draw Time: %fms", input.frameDrawTime);

            static constexpr uint SAMPLE_COUNT = 200;
            static constexpr uint REFRESH_RATE = 60;

            static uint currentSampleIndex = 0;

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
            for (int i = SAMPLE_COUNT; i--;) averageDrawTime += drawTimeSamples[i];
            averageDrawTime /= (float) SAMPLE_COUNT;

            float averageFrameTime = 0.0f;
            for (int i = SAMPLE_COUNT; i--;) averageFrameTime += frameTimeSamples[i];
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

    // ********************* System Debug Panel ********************* \\

    void SystemDebugPanel::DrawUI()
    {
        if (GUI::BeginWindow("System Debugger", WindowFlags::NO_FOCUS_ON_APPEARING))
        {
            if (GUI::BeginTree("CPU"))
            {
                auto &cpu = SystemInformation::GetCPU();
                GUI::Text("Name: %s", cpu.name.c_str());
                GUI::Text("Endianness: %s", cpu.GetEndiannessString());
                GUI::Text("Architecture: %s", cpu.GetArchitectureString());

                GUI::VerticalIndent(1.0f);
                if (GUI::BeginTree("Physical Information"))
                {
                    GUI::Text("Physical core count: %u", cpu.physicalInformation.physicalCoreCount);
                    GUI::Text("Logical core count: %u", cpu.physicalInformation.logicalCoreCount);
                    GUI::Text("Threads per core: %u", cpu.physicalInformation.threadsPerCore);
                    GUI::Text("Frequency: %llu", cpu.physicalInformation.frequency);
                    GUI::EndTree();
                }
                GUI::EndTree();
            }

            GUI::VerticalIndent();
            if (GUI::BeginTree("GPU"))
            {
                auto &gpu = SystemInformation::GetGPU();
                GUI::Text("Name: %s", gpu.name.c_str());
                GUI::Text("Vendor: %s", gpu.GetVendorString());

                GUI::VerticalIndent(1.0f);
                if (GUI::BeginTree("Physical Information"))
                {
                    GUI::Text("Total VRAM: %lluMB", gpu.physicalInformation.totalMemory / 1000000);
                    GUI::Text("Used VRAM: %lluMB", SystemInformation::GetGPU().physicalInformation.GetUsedVideoMemory() / 1000000);
                    GUI::EndTree();
                }
                GUI::EndTree();
            }

            GUI::VerticalIndent();
            if (GUI::BeginTree("Memory"))
            {
                auto &memory = SystemInformation::GetMemory();
                GUI::Text("Total physical memory: %lluMB", memory.totalPhysicalMemory / 1000000);
                GUI::Text("Total virtual memory: %lluMB", memory.totalVirtualMemory / 1000000);
                GUI::Text("Available physical memory: %lluMB", memory.GetAvailablePhysicalMemory() / 1000000);
                GUI::Text("Available virtual memory: %lluMB", memory.GetAvailableVirtualMemory() / 1000000);
                GUI::EndTree();
            }

            GUI::VerticalIndent();
            if (GUI::BeginTree("Kernel"))
            {
                auto &kernel = SystemInformation::GetKernel();
                GUI::Text("Type: %s", kernel.GetTypeString());
                GUI::Text("Version: %u.%u.%u", kernel.version.major, kernel.version.minor, kernel.version.patch);
                GUI::Text("Build number: %u", kernel.buildNumber);
                GUI::EndTree();
            }

            GUI::VerticalIndent();
            if (GUI::BeginTree("Operating System"))
            {
                auto &os = SystemInformation::GetOperatingSystem();
                GUI::Text("Name: %s", os.name);
                GUI::Text("Version: %u.%u.%u", os.version.major, os.version.minor, os.version.patch);
                GUI::Text("Build number: %u", os.buildNumber);
                GUI::EndTree();
            }
        }
        GUI::EndWindow();
    }

    // ********************* Game Pad Debug Panel ********************* \\

    void GamePadDebugPanel::DrawUI()
    {
        for (uint i = Input::MAX_GAME_PADS; i--;)
        {
            if (Input::GetGamePadConnected(i))
            {
                if (GUI::BeginWindow((FORMAT_STRING("Game Pad [{0}] Data", i).c_str()), WindowFlags::ALWAYS_AUTO_RESIZE))
                {
                    GUI::Text("%s", ("Game pad [" + Input::GetGamePadName(i) + "] properties:").c_str());
                    GUI::Text("Left gamepad stick: [%f, %f]", Input::GetGamePadLeftStickAxis(i).x, Input::GetGamePadLeftStickAxis(i).y);
                    GUI::Text("Right gamepad stick: [%f, %f]", Input::GetGamePadRightStickAxis(i).x, Input::GetGamePadRightStickAxis(i).y);
                    GUI::Text("Left trigger: [%f]", Input::GetGamePadLeftTriggerAxis(i));
                    GUI::Text("Right trigger: [%f]", Input::GetGamePadRightTriggerAxis(i));
                    GUI::RadioButton("\"A\" pressed", Input::GetGamePadButtonPressed(GamePadButton::A, i));
                    GUI::RadioButton("\"A\" held", Input::GetGamePadButtonHeld(GamePadButton::A, i));
                    GUI::RadioButton("\"A\" released", Input::GetGamePadButtonReleased(GamePadButton::A, i));
                }
                GUI::EndWindow();
            }
        }
    }

    // ********************* Discord Debug Panel ********************* \\

    void DiscordDebugPanel::DrawUI()
    {
        if (!Discord::IsInitialized() || !Discord::GetUser().IsLoaded() || !Discord::GetUser().IsIconTextureLoaded()) return;

        if (GUI::BeginWindow("Discord Debugger", WindowFlags::ALWAYS_AUTO_RESIZE | WindowFlags::NO_FOCUS_ON_APPEARING))
        {
            GUI::PushBoldFont();
            GUI::Text("%s", "Detected User:");
            GUI::PopFont();

            GUI::ContinueOnSameLine();
            GUI::Text("%s#%s", Discord::GetUser().GetUsername(), Discord::GetUser().GetDiscriminator());

            GUI::Texture(Discord::GetUser().GetIconTexture()->GetImGuiTextureID(), {
                    GUI::GetRemainingHorizontalSpace() / 3.5f,
                                                                                     GUI::GetRemainingHorizontalSpace() / 3.5f });
            GUI::VerticalIndent(5.0f);

            GUI::PushBoldFont();
            GUI::Text("User Data:");
            GUI::PopFont();

            GUI::Text("Username: %s", Discord::GetUser().GetUsername());
            GUI::Text("Discriminator: %s", Discord::GetUser().GetDiscriminator());
            GUI::Text("ID: %llu", Discord::GetUser().GetId());

        }
        GUI::EndWindow();
    }
}