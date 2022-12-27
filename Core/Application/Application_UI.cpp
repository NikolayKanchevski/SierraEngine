//
// Created by Nikolay Kanchevski on 19.10.22.
//

#include "Application.h"

#include <imgui.h>
#include <imgui_internal.h>

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
                                renderer->GetPipeline()->OverloadShader(Shader::Create({
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

    /* --- SCENE VIEW --- */
    {
        // Create scene view tab
        if (ImGui::Begin("Scene View", nullptr))
        {
            // Get and show current renderer image
            ImVec2 freeSpace = ImGui::GetContentRegionAvail();
            ImGuiCore::SetSceneViewSize(freeSpace.x, freeSpace.y);

            ImGui::Image((ImTextureID) renderer->GetRenderedTextureDescriptorSet(), freeSpace);
            ImGui::End();
        }
    }

}

void Application::ListDeeper(Relationship &relationship, const uint32_t iteration)
{
    ImGuiTreeNodeFlags treeNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

    bool selected = false;
    if (!EngineCore::GetSelectedEntity().IsNull() && EngineCore::GetSelectedEntity().GetComponent<UUID>() == relationship.GetComponent<UUID>())
    {
        selected = true;
        treeNodeFlag |= ImGuiTreeNodeFlags_Selected;
        ImGui::PushStyleColor(ImGuiCol_Header, { 0.11f, 0.32f, 0.5f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, { 0.1f, 0.26f, 0.45f, 1.0f });
    }

    bool opened = ImGui::TreeNodeEx(std::string(relationship.GetComponent<Tag>().tag + "##" + std::to_string(relationship.GetComponent<UUID>().GetValue())).c_str(), treeNodeFlag);

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