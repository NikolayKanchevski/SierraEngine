//
// Created by Nikolay Kanchevski on 4.11.25.
//

#include "MenuPanel.h"

namespace SierraEngine
{

    namespace
    {
        void DrawItem(const MenuItem& item)
        {
            if (item.Callback == nullptr)
            {
                if (ImGui::BeginMenu(item.title.data(), item.enabled))
                {
                    for (const MenuItem& child : item.items)
                    {
                        DrawItem(child);
                    }

                    ImGui::EndMenu();
                }
            }
            else
            {
                if (ImGui::MenuItem(item.title.data()))
                {
                    item.Callback();
                }
            }
        }
    }

    /* --- POLLING METHODS --- */

    void MenuPanel::Draw(const MenuPanelDrawInfo& drawInfo)
    {
        if (ImGui::BeginMenuBar())
        {
            for (const MenuItem& item : drawInfo.items)
            {
                DrawItem(item);
            }
            ImGui::EndMenuBar();
        }
    }

}