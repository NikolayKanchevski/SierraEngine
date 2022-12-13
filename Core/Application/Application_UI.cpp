//
// Created by Nikolay Kanchevski on 19.10.22.
//


#include "Application.h"

void Application::DisplayUI(VulkanRenderer &renderer)
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
        bool debugInfoOpen = true;

        // Create debug tab
        if (ImGui::Begin("Debug Information", &debugInfoOpen, ImGuiWindowFlags_NoNav) || debugInfoOpen)
        {
            ImGui::Text("CPU Frame Time: %i FPS", Time::GetFPS());
            ImGui::Text("GPU Draw Time: %f ms", renderer.GetRendererInfo().drawTime);
            ImGui::Separator();
            ImGui::Text("Total meshes being drawn: %i", renderer.GetRendererInfo().meshesDrawn);
            ImGui::Text("Total vertices in scene: %i", renderer.GetRendererInfo().verticesDrawn);

            ImGui::End();
        }
    }

    /* --- HIERARCHY --- */
    #ifdef DRAW_IMGUI_HIERARCHY
    {
        bool hierarchyOpen = true;

        // Create hierarchy tab
        if (ImGui::Begin("Hierarchy", &hierarchyOpen, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_HorizontalScrollbar) || hierarchyOpen)
        {
            ImGui::Separator();

            // Recursively show all entities in the hierarchy
            auto enttRelationshipView = World::GetEnttRegistry().view<Relationship>();
            for (const auto &entity: enttRelationshipView)
            {
                Relationship &entityRelationship = World::GetEnttRegistry().get<Relationship>(entity);
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
        bool rendererViewOpen = true;

        // Create scene view tab
        if (ImGui::Begin("Scene View", &rendererViewOpen) || rendererViewOpen)
        {
            // Get and show current renderer image
            ImVec2 freeSpace = ImGui::GetContentRegionAvail();

            float imageAspectRatio = (float) VulkanCore::GetWindow()->GetWidth() / (float) VulkanCore::GetWindow()->GetHeight();
            ImVec2 size = { freeSpace.x, freeSpace.x / imageAspectRatio };

            ImGui::Image((ImTextureID) renderer.GetRenderedTextureDescriptorSet(), size);
            ImGui::End();
        }
    }

}

void Application::ListDeeper(Relationship &relationship, const uint32_t iteration)
{
    ImGuiTreeNodeFlags treeNodeFlag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;

    if (ImGui::TreeNodeEx(relationship.GetComponent<Tag>().tag.c_str(), treeNodeFlag))
    {
        for (auto child : relationship.GetEnttChildrenEntities())
        {
            Relationship &childRelationship = World::GetEnttRegistry().get<Relationship>(child);
            ListDeeper(childRelationship, iteration + 1);
        }

        ImGui::TreePop();
    }

    if (iteration == 0) ImGui::Separator();
}