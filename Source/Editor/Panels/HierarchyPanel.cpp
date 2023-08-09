//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "HierarchyPanel.h"

#include "../GUI.h"
#include "../Editor.h"
#include "../../Engine/Classes/Entity.h"
#include "../../Engine/Components/Tag.h"
#include "../../Engine/Components/Relationship.h"

namespace Sierra::Editor
{
    using namespace Engine;

    /* --- POLLING METHODS --- */

    void ListDeeper(Relationship &relationship, const uint32 iteration)
    {
        TreeFlags treeFlags = TreeFlags::OPEN_ON_ARROW | TreeFlags::FRAME_PADDING | TreeFlags::SPAN_AVAILABLE_WIDTH;

        bool selected = false;
        if (Editor::GetSelectedEntity() == relationship.GetEntity())
        {
//            PRINT(relationship.GetEntity().GetTag());
            selected = true;
            treeFlags |= TreeFlags::SELECTED;
            GUI::PushStyleColor(StyleColor::HEADER,         { 0.11f, 0.32f, 0.50f, 1.00f });
            GUI::PushStyleColor(StyleColor::HOVERED_HEADER, { 0.10f, 0.26f, 0.45f, 1.00f });
        }

        bool opened = GUI::BeginTree((void*)(uintptr_t) relationship.GetEntity().GetID(), treeFlags, "%s", relationship.GetComponent<Tag>().tag.c_str());

        if (GUI::IsItemClicked())
        {
            Editor::SetSelectedEntity(relationship.GetEntity());
        }

        if (selected)
        {
            GUI::PopStyleColor(2);
        }

        if (opened)
        {
            for (auto child : relationship.GetChildren())
            {
                Relationship &childRelationship = child.GetComponent<Relationship>();
                ListDeeper(childRelationship, iteration + 1);
            }

            GUI::EndTree();
        }


        if (iteration == 0) GUI::InsertSeparator();
    }

    void HierarchyPanel::DrawUI()
    {
         // Create hierarchy tab
        if (GUI::BeginWindow("Hierarchy", WindowFlags::NO_NAV | WindowFlags::SHOW_HORIZONTAL_SCROLLBAR))
        {
            GUI::InsertSeparator();

            // Recursively show all entities in the hierarchy
            for (const auto &entity : World::GetOriginEntities())
            {
                ListDeeper(World::GetComponent<Relationship>(entity), 0);
            }
        }
        GUI::EndWindow();
    }
}

