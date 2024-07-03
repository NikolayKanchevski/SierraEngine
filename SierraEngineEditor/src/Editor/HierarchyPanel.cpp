//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "HierarchyPanel.h"

namespace SierraEngine
{

    namespace
    {
        void DrawEntity(const Entity entity, Entity &selectedEntity, const bool rootEntity = true)
        {
            const bool opened = ImGui::TreeNodeEx(entity.GetTag().data(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | ((entity == selectedEntity) * ImGuiTreeNodeFlags_Selected) | ((entity.GetChildCount() == 0) * ImGuiTreeNodeFlags_Leaf), "%s", entity.GetTag().data());
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) selectedEntity = entity;

            if (opened)
            {
                for (uint32 i = 0; i < entity.GetChildCount(); i++)
                {
                    DrawEntity(entity.GetChild(i), selectedEntity, false);
                }

                ImGui::TreePop();
            }

            if (rootEntity) ImGui::Separator();
        }
    }

    /* --- POLLING METHODS --- */

    void HierarchyPanel::Draw(Scene &scene, Entity &selectedEntity)
    {
        if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoNav | ImGuiWindowFlags_HorizontalScrollbar))
        {
            ImGui::Separator();

            // Recursively show all entities in the hierarchy
            scene.ForEachEntity([&selectedEntity](const Entity entity) -> void { if (!entity.HasParent()) DrawEntity(entity, selectedEntity); });

            ImGui::End();
        }
    }

}