//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "HierarchyPanel.h"

namespace SierraEngine
{

    namespace
    {
        void DrawEntity(const Scene& scene, const EntityID entityID, EntityID& selectedEntity)
        {
            const std::span<const EntityID> children = scene.GetEntityChildren(entityID);
            const bool opened = ImGui::TreeNodeEx(std::to_string(entityID.GetHash()).data(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ((entityID == selectedEntity) * ImGuiTreeNodeFlags_Selected) | (children.empty() * ImGuiTreeNodeFlags_Leaf), "%s", scene.GetEntityComponent<Tag>(entityID)->GetTag().data());

            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) selectedEntity = entityID;
            if (opened)
            {
                for (const EntityID child : children)
                {
                    DrawEntity(scene, child, selectedEntity);
                }

                ImGui::TreePop();
            }
        }
    }

    /* --- POLLING METHODS --- */

    void HierarchyPanel::Draw(Scene& scene, EntityID& selectedEntity)
    {
        if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::Separator();

            for (const EntityID entityID : scene.GetRootEntities())
            {
                DrawEntity(scene, entityID, selectedEntity);
                ImGui::Separator();
            }

            ImGui::End();
        }
    }

}