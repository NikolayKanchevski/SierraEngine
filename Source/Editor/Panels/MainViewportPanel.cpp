//
// Created by Nikolay Kanchevski on 30.07.23.
//

#include "MainViewportPanel.h"

#include "../GUI.h"

namespace Sierra::Editor
{
    /* --- POLLING METHODS --- */

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
        GUI::PushStyleVariable(StyleVariable::WINDOW_ROUNDING, 0.0f);

        // Create main viewport window
        GUI::BeginWindow("Viewport", windowFlags);
        GUI::PopStyleVariable(2);

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
}