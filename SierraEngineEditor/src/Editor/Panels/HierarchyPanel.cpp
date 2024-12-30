//
// Created by Nikolay Kanchevski on 15.05.24.
//

#include "HierarchyPanel.h"

namespace SierraEngine
{

    /* --- POLLING METHODS --- */

    void HierarchyPanel::Draw(const Scene& scene)
    {
        if (ImGui::Begin("Hierarchy", nullptr, DEFAULT_WINDOW_FLAGS))
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

    /* --- POLLING METHODS --- */

    void HierarchyPanel::DrawEntity(const Scene& scene, const EntityID entityID)
    {
        const auto iterator = std::find(selectedEntities.begin(), selectedEntities.end(), entityID);
        const bool selected = iterator != selectedEntities.end();

        const std::span<const EntityID> children = scene.GetEntityChildren(entityID);
        const bool opened = ImGui::TreeNodeEx(std::to_string(entityID.GetValue()).data(), ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth | (selected * ImGuiTreeNodeFlags_Selected) | (children.empty() * ImGuiTreeNodeFlags_Leaf), "%s", scene.GetEntityComponent<Tag>(entityID)->GetTag().data());

        if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
        {
            if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_LeftSuper))
            {
                if (!selected) selectedEntities.emplace_back(entityID);
                else selectedEntities.erase(iterator);
            }
            else
            {
                selectedEntities = { entityID };
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