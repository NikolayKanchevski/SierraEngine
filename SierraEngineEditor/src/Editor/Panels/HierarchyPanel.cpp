//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "HierarchyPanel.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void HierarchyPanel::Draw(Scene& scene)
    {
        if (ImGui::Begin("Hierarchy", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoNav))
        {
            ImGui::Separator();

            for (const EntityID entityID : scene.GetRootEntities())
            {
                DrawEntity(scene, entityID);
                ImGui::Separator();
            }
        }
        ImGui::End();
    }

    /* --- PRIVATE METHODS --- */

    void HierarchyPanel::DrawEntity(Scene& scene, EntityID entity)
    {
        const auto iterator = std::find(selectedEntities.begin(), selectedEntities.end(), entity);
        const bool selected = iterator != selectedEntities.end();

        const std::span<const EntityID> children = scene.GetEntityChildren(entity);
        const bool opened = ImGui::TreeNodeEx(std::to_string(entity.GetValue()).data(), ImGuiTreeNodeFlags_DrawLinesToNodes | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | (selected * ImGuiTreeNodeFlags_Selected) | (children.empty() * ImGuiTreeNodeFlags_Leaf), "%s", scene.GetEntityComponent<Tag>(entity)->GetTag().data());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftSuper))
            {
                if (!selected) selectedEntities.emplace_back(entity);
                else selectedEntities.erase(iterator);
            }
            else
            {
                selectedEntities = { entity };
            }
        }

        if (opened)
        {
            for (const EntityID child : children)
            {
                DrawEntity(scene, child);
            }

            ImGui::TreePop();
        }
    }


}