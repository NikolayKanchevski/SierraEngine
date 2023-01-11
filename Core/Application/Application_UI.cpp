//
// Created by Nikolay Kanchevski on 19.10.22.
//

#include "Application.h"

#include <imgui.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

#include "../../Core/Rendering/Math/MatrixUtilities.h"
#include "../../Core/Rendering/UI/ImGuiCore.h"

using UI::ImGuiCore;

void Application::DisplayUI(std::unique_ptr<MainVulkanRenderer> &renderer)
{

    /* --- MAIN VIEWPORT --- */
    {
        // Set up dock space and window flags
        ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground |
                                       ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                       ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // Get a pointer to the main viewport of ImGui
        const ImGuiViewport *viewport = ImGui::GetMainViewport();

        // Set window sizing properties accordingly
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);

        // Get the docking of the main viewport
        ImGuiID dockSpaceID = ImGui::GetID("ViewportDock");

        // Disable window padding
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        // Create main viewport window
        ImGui::Begin("Viewport", nullptr, windowFlags);

        // Use dock space
        ImGui::DockSpace(dockSpaceID, ImVec2(0.0f, 0.0f), dockSpaceFlags);

        // Create menu bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Preferences"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("I wanna die"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }

        // Remove the overridden window padding
        ImGui::PopStyleVar(1);

        // Finalize main viewport window
        ImGui::End();
    }

    /* --- DEBUG INFORMATION --- */
    {
        // Create debug tab
        if (ImGui::Begin("Debug Information", nullptr, ImGuiWindowFlags_NoNav))
        {
            ImGui::Text("CPU Frame Time: %i FPS", Time::GetFPS());
            ImGui::Text("GPU Draw Time: %f ms", renderer->GetTotalDrawTime());
            ImGui::Separator();
            ImGui::Text("Total meshes being drawn: %i", Mesh::GetTotalMeshCount());
            ImGui::Text("Total vertices in scene: %llu", renderer->GetTotalVerticesDrawn());

            ImGui::End();
        }
    }

    /* --- RENDERER SETTINGS --- */
    {
        if (ImGui::Begin("Renderer Settings", nullptr, ImGuiWindowFlags_NoNav))
        {
            // Shading type dropdown
            {
                static const char* currentShading = "Shaded";
                static const char* shadingTypes[] = {"Shaded", "Wireframe" };

                ImGui::Text("Renderer Shading:");
                ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                if (ImGui::BeginCombo("RENDERER_SHADING##combo", currentShading)) // The second parameter is the label previewed before opening the combo.
                {
                    for (int i = 0; i < IM_ARRAYSIZE(shadingTypes); i++)
                    {
                        bool selected = (currentShading == shadingTypes[i]);

                        if (ImGui::Selectable(shadingTypes[i], selected) && currentShading != shadingTypes[i])
                        {
                            currentShading = shadingTypes[i];
                            renderer->SetShadingType((ShadingType) i);
                        }
                        if (selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }

                // Shaders to use
                {
                    static const char* currentShader = "Blinn-Phong";
                    static const char* shaderTypes[] = {"Diffuse", "Specular", "Blinn-Phong" };
                    static const char* shaderPaths[]
                    { "Shaders/standard_diffuse_fragment.frag.spv", "Shaders/standard_specular_fragment.frag.spv", "Shaders/blinn-phong-fragment.frag.spv" };

                    ImGui::Text("Renderer's Fragment Shader:");
                    ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                    if (ImGui::BeginCombo("RENDERER_FRAGMENT_SHADER##combo", currentShader))
                    {
                        for (int i = 0; i < IM_ARRAYSIZE(shaderTypes); i++)
                        {
                            bool selected = (currentShader == shaderTypes[i]);

                            if (ImGui::Selectable(shaderTypes[i], selected) && currentShader != shaderTypes[i])
                            {
                                currentShader = shaderTypes[i];
                                renderer->GetScenePipeline()->OverloadShader(Shader::Create({
                                    .filePath = shaderPaths[i],
                                    .shaderType = FRAGMENT_SHADER
                                }));
                            }
                            if (selected)
                            {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    // Anti-Aliasing
                    {
                        static const char* msaaTypes[] = {"None", "MSAAx2", "MSAAx4", "MSAAx8", "MSAAx16", "MSAAx32", "MSAAx64" };
                        static const char* currentMSAA = "None";

                        ImGui::Text("Renderer Anti-Aliasing:");
                        ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionWidth());
                        if (ImGui::BeginCombo("RENDERER_MSAA##combo", currentMSAA))
                        {
                            for (int i = 0; i < IM_ARRAYSIZE(msaaTypes); i++)
                            {
                                Sampling currentSampling = (Sampling) glm::pow(2, i);
                                if (currentSampling > VK::GetDevice()->GetHighestMultisampling())
                                {
                                    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
                                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                                }

                                bool selected = (currentMSAA == msaaTypes[i]);

                                if (ImGui::Selectable(msaaTypes[i], selected) && currentMSAA != msaaTypes[i])
                                {
                                    currentMSAA = msaaTypes[i];
                                    renderer->SetSampling(currentSampling);
                                }
                                if (selected)
                                {
                                    ImGui::SetItemDefaultFocus();
                                }

                                if (currentSampling > VK::GetDevice()->GetHighestMultisampling())
                                {
                                    ImGui::PopItemFlag();
                                    ImGui::PopStyleVar();
                                }
                            }
                            ImGui::EndCombo();
                        }
                    }
                }
            }

            ImGui::End();
        }

    }

    /* --- SCENE VIEW --- */

    // Save scene draw list, so it can be clipped for gizmos
    ImDrawList *sceneDrawList;
    {
        // Create scene view tab
        if (ImGui::Begin("Scene View", nullptr))
        {
            sceneDrawList = ImGui::GetWindowDrawList();

            // Get and show current renderer image
            ImVec2 freeSpace = ImGui::GetContentRegionAvail();
            ImGuiCore::SetSceneViewSize(freeSpace.x, freeSpace.y);
            ImGuiCore::SetSceneViewPosition(ImGui::GetCurrentWindow()->WorkRect.GetTL().x, ImGui::GetCurrentWindow()->WorkRect.GetTL().y);

            // Flip renderer image
            ImGui::Image((ImTextureID) renderer->GetRenderedTextureDescriptorSet(), freeSpace, { 0.0f, 1.0f }, { 1.0f, 0.0f });
            ImGui::End();
        }
    }

    /* --- IMGUIZMO --- */
    {
        if (!renderer->GetImGuiInstance()->HasImGuizmoLayer()) return;

        // Prepare ImGuizmo
        ImGuizmo::Enable(true);
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetRect(ImGuiCore::GetSceneViewPositionX(), ImGuiCore::GetSceneViewPositionY(), ImGuiCore::GetSceneViewWidth(), ImGuiCore::GetSceneViewHeight());
        ImGuizmo::SetDrawlist(sceneDrawList);

        // Set gizmo mode
        static ImGuizmo::OPERATION operation = ImGuizmo::OPERATION::TRANSLATE;
        if (Input::GetKeyPressed(GLFW_KEY_C)) operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE | ImGuizmo::SCALE;
        if (Input::GetKeyPressed(GLFW_KEY_R)) operation = ImGuizmo::SCALE;
        if (Input::GetKeyPressed(GLFW_KEY_E)) operation = ImGuizmo::ROTATE;
        if (Input::GetKeyPressed(GLFW_KEY_W)) operation = ImGuizmo::TRANSLATE;

        // Set the renderer's context
        ImGuizmo::SetImGuiContext(renderer->GetImGuiInstance()->GetImGuiContext());

        // Get a pointer to the main camera
        auto mainCamera = Camera::GetMainCamera();

        // Convert camera's view matrix to array data
        glm::mat4x4 viewMatrix = mainCamera->GetViewMatrix();
        glm::mat4x4 projectionMatrix = mainCamera->GetProjectionMatrix();

        // Show cube view
        ImGuizmo::ViewManipulate(glm::value_ptr(viewMatrix), 10.0f, { ImGuiCore::GetSceneViewPositionX() + ImGuiCore::GetSceneViewWidth() - renderer->GetWindow()->GetWidth() / 11.25f, ImGuiCore::GetSceneViewPositionY() }, { renderer->GetWindow()->GetWidth() / 11.25f, renderer->GetWindow()->GetWidth() / 11.25f }, 0x0000000);

        // Decompose modified view matrix to get new yaw and pitch
        glm::mat4 inverted = glm::inverse(viewMatrix);
        glm::vec3 direction = -glm::vec3(inverted[2]);

        // Update the local yaw and pitch
        float deltaYaw = glm::degrees(glm::atan(direction.z, direction.x)) - yaw;
        yaw += deltaYaw;

        float deltaPitch = glm::degrees(glm::asin(direction.y)) - pitch;
        pitch += deltaPitch;

        pitch = Math::Clamp(pitch, -85.0f, 85.0f);

        // Apply yaw and pitch changes to the camera
        Transform &cameraTransform = camera.GetComponent<Transform>();
        cameraTransform.rotation.x = yaw;
        cameraTransform.rotation.y = pitch;

        // If an object is selected
        if (!EngineCore::GetSelectedEntity().IsNull())
        {
            // Scissor gizmos so they don't go beyond the window
            sceneDrawList->PushClipRect({ ImGuiCore::GetSceneViewPositionX(), ImGuiCore::GetSceneViewPositionY() }, { ImGuiCore::GetSceneViewPositionX() + ImGuiCore::GetSceneViewWidth(), ImGuiCore::GetSceneViewPositionY() + ImGuiCore::GetSceneViewHeight() });

            Transform &transform = EngineCore::GetSelectedEntity().GetTransform();

            // Convert object's transform into an array
            glm::mat4x4 modelMatrix = Matrix::CreateModel({ transform.position.x, -transform.position.y, transform.position.z }, transform.rotation, transform.scale);

            // Set snapping
            float snapDeterminant = Input::GetKeyHeld(GLFW_KEY_LEFT_SHIFT) ? (operation == ImGuizmo::OPERATION::ROTATE ? 45.0f : 0.5f) : 0.0f;
            float snapping[3] = { snapDeterminant, snapDeterminant, snapDeterminant };

            // Show gizmos
            ImGuizmo::Manipulate(glm::value_ptr(viewMatrix), glm::value_ptr(projectionMatrix), operation, ImGuizmo::MODE::LOCAL, glm::value_ptr(modelMatrix), nullptr, snapping);

            // Retrieve and apply gizmo's changes
            if (ImGuizmo::IsUsing())
            {
                // Decompose model matrix
                glm::vec3 translation, rotation, scale;
                if (!Matrix::DecomposeModelMatrix(modelMatrix, translation, rotation, scale)) return;

                // Swap X and Y rotation to suit the engine's axis
                float y = rotation.y;
                rotation.y = rotation.x;
                rotation.x = y;

                // Apply position, rotation, and scale changes whilst inverting Y position, as it has already been inverted once to suit Vulkan's -Y needs
                transform.position = { translation.x, -translation.y, translation.z };
                transform.rotation = rotation;
                transform.scale = scale;
            }
        }
    }

    /* --- PROPERTIES --- */
    {
        if (ImGui::Begin("Properties", nullptr, ImGuiWindowFlags_NoNav))
        {
            if (!EngineCore::GetSelectedEntity().IsNull())
            {
                Entity &selectedEntity = EngineCore::GetSelectedEntity();

                selectedEntity.GetComponent<UUID>().DrawUI();
                selectedEntity.GetComponent<Tag>().DrawUI();
                selectedEntity.GetComponent<Transform>().DrawUI();
            }

            ImGui::End();
        }
    }

    /* --- HIERARCHY --- */
    #ifdef DRAW_IMGUI_HIERARCHY
    {
        // Create hierarchy tab
        if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::Separator();

            // Recursively show all entities in the hierarchy
            auto enttRelationshipView = World::GetEnttRegistry()->view<Relationship>();
            for (const auto &entity: enttRelationshipView)
            {
                Relationship &entityRelationship = World::GetEnttRegistry()->get<Relationship>(entity);
                if (entityRelationship.GetEnttParentEntity() == entt::null)
                {
                    ListDeeper(entityRelationship, 0);
                }
            }

            ImGui::End();
        }
    }
    #endif

    /* --- GAMEPAD STATISTICS --- */
    {
        for (int i = Input::MAX_GAME_PADS; i--;)
        {
            if (Input::GetGamePadConnected(i))
            {
                bool gamePadInfoOpen = true;
                if (ImGui::Begin((("Game Pad [" + std::to_string(i) + "] Data").c_str()), &gamePadInfoOpen,
                                 ImGuiWindowFlags_AlwaysAutoResize))
                {
                    ImGui::Text("%s", ("Game pad [" + Input::GetGamePadName(i) + "] properties:").c_str());
                    ImGui::Text("%s", ("Left gamepad stick: [" + std::to_string(Input::GetGamePadLeftStickAxis(i).x) +
                                       " || " + std::to_string(Input::GetGamePadLeftStickAxis(i).y) + "]").c_str());
                    ImGui::Text("%s", ("Right gamepad stick: [" + std::to_string(Input::GetGamePadRightStickAxis(i).x) +
                                       " || " + std::to_string(Input::GetGamePadRightStickAxis(i).y) + "]").c_str());
                    ImGui::Text("%s", ("Left trigger: [" + std::to_string(Input::GetGamePadLeftTriggerAxis(i)) +
                                       "]").c_str());
                    ImGui::Text("%s", ("Right trigger: [" + std::to_string(Input::GetGamePadRightTriggerAxis(i)) +
                                       "]").c_str());
                    ImGui::RadioButton("\"A\" pressed", Input::GetGamePadButtonPressed(GLFW_GAMEPAD_BUTTON_A, i));
                    ImGui::RadioButton("\"A\" held", Input::GetGamePadButtonHeld(GLFW_GAMEPAD_BUTTON_A, i));
                    ImGui::RadioButton("\"A\" released", Input::GetGamePadButtonReleased(GLFW_GAMEPAD_BUTTON_A, i));

                    ImGui::End();
                }
            }
        }
    }
}

void Application::ListDeeper(Relationship &relationship, const uint32_t iteration)
{
    ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

    bool selected = false;
    if (!EngineCore::GetSelectedEntity().IsNull() && EngineCore::GetSelectedEntity().GetComponent<UUID>() == relationship.GetComponent<UUID>())
    {
        selected = true;
        treeNodeFlags |= ImGuiTreeNodeFlags_Selected;
        ImGui::PushStyleColor(ImGuiCol_Header, { 0.11f, 0.32f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.1f, 0.26f, 0.45f, 1.0f });
    }

    bool opened = ImGui::TreeNodeEx((void*) relationship.GetComponent<UUID>().GetValue(), treeNodeFlags, "%s", relationship.GetComponent<Tag>().tag.c_str());

    if (ImGui::IsItemClicked())
    {
        EngineCore::SetSelectedEntity(Entity(relationship.GetEnttEntity()));
    }

    if (selected)
    {
        ImGui::PopStyleColor(2);
    }

    if (opened)
    {
        for (auto child : relationship.GetEnttChildrenEntities())
        {
            Relationship &childRelationship = World::GetEnttRegistry()->get<Relationship>(child);
            ListDeeper(childRelationship, iteration + 1);
        }

        ImGui::TreePop();
    }


    if (iteration == 0) ImGui::Separator();
}