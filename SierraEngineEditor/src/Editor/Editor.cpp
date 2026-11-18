//
// Created by Nikolay Kanchevski on 3.08.24.
//

#include "Editor.h"

#include "Panels/MenuPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Editor::Editor(const ApplicationFrame& applicationFrame, const EditorCreateInfo& createInfo)
        : scene({ .name = "Scene" }),
          assetBrowserPanel(applicationFrame, { .rootDirectoryPath = applicationFrame.project.GetAssetPath() }),
          logPanel({ .logger = Sierra::Logger::GetApplicationLogger() })
    {
        style = EditorThemeToImGuiStyle(createInfo.theme);

        createInfo.window.AddEventListener<Sierra::WindowFocusEvent>([this](const Sierra::WindowFocusEvent& event) -> bool
        {
            refreshAssetBrowserPanel = event.IsFocused();
            return false;
        });
    }

    /* --- POLLING METHODS --- */

    void Editor::Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame)
    {
        ImGui::GetStyle() = style;

        const EditorFrame editorFrame = Bind();
        const ImGuiID dockID = DrawDockSpace(applicationFrame, editorFrame);

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        hierarchyPanel.Draw(scene);

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        PropertiesPanel::Draw({ .scene = scene, .selectedEntity = hierarchyPanel.GeSelectedEntity() });

        if (refreshAssetBrowserPanel)
        {
            assetBrowserPanel.Refresh(applicationFrame);
            refreshAssetBrowserPanel = false;
        }

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        assetBrowserPanel.Draw(applicationFrame, editorFrame);

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        logPanel.Draw();

        uint32 i = 0;
        viewports.ForEach([dockID, &i, &applicationFrame](Viewport& viewport) -> void
        {
            const std::string title = SR_FORMAT("Viewport [{0}]", i);
            const ViewportPanelDrawInfo drawInfo
            {
                .title = title,
                .viewport = viewport,
                .timeStep = applicationFrame.timeStep
            };

            ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
            ViewportPanel::Draw(drawInfo);
            i++;
        });

        modalManager.Draw(applicationFrame, drawFrame);
    }

    Viewport& Editor::CreateViewport(ViewportID& ID, const ViewportCreateInfo& createInfo)
    {
        return viewports.AddItem(ID, createInfo);
    }

    void Editor::ForEachViewport(const ViewportEnumerationPredicate& Predicate)
    {
        viewports.ForEach(Predicate);
    }

    bool Editor::DestroyViewport(const ViewportID ID)
    {
        return viewports.RemoveItem(ID);
    }

    /* --- PRIVATE METHODS --- */

    EditorFrame Editor::Bind()
    {
        return
        {
            .modalManager = modalManager
        };
    }

    ImGuiID Editor::DrawDockSpace(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame) const
    {
        const ImGuiID dockID = ImGui::GetID("Dockspace");

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        // Create docked viewport
        ImGui::Begin("##BackgroundViewport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);
        ImGui::PopStyleVar(2);

        MenuPanel::Draw(applicationFrame, editorFrame);

        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
        ImGui::DockSpace(dockID, { 0.0f, 0.0f }, static_cast<ImGuiDockNodeFlags>(ImGuiDockNodeFlags_NoWindowMenuButton) | ImGuiDockNodeFlags_PassthruCentralNode);
        #pragma clang diagnostic pop

        ImGui::End();

        return dockID;
    }


}