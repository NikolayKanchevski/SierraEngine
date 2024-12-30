//
// Created by Nikolay Kanchevski on 3.08.24.
//

#include "Editor.h"

//#include "Wizards/TextureSerializeWizard.h"

namespace SierraEngine
{

    namespace
    {
        constexpr std::string_view ASSET_DATA_EXTENSION = ".data";

        constexpr std::array<std::string_view, 8> TEXTURE_FILE_EXTENSIONS = { ".jpg", ".jpeg", ".png", ".bmp", ".psd", ".tga", ".gif", ".hdr" };
        constexpr std::string_view TEXTURE_ASSET_EXTENSION = ".texture";
    }

    /* --- CONSTRUCTORS --- */

    Editor::Editor(const EditorCreateInfo& createInfo)
        : device(createInfo.device), platformContext(createInfo.platformContext), resourceTable(createInfo.resourceTable), scene(createInfo.scene)
    {
        style = EditorThemeToImGuiStyle(createInfo.theme);
    }

    /* --- POLLING METHODS --- */

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "bugprone-suspicious-enum-usage"
    #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"

    void Editor::Draw()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        // Assign style
        ImGui::GetStyle() = style;

        // Create background viewport and draw menu bar
        const ImGuiID dockID = ImGui::GetID("Dockspace");
        {
            const ImGuiViewport* const viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

            // Create docked viewport
            ImGui::Begin("Background Viewport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
            ImGui::DockSpace(dockID, { 0.0f, 0.0f }, ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_PassthruCentralNode);
            ImGui::End();

            ImGui::PopStyleVar(2);
        }

        if (currentWizard != nullptr)
        {
            bool open = true;
            currentWizard->Draw(open);

            if (!open)
            {
                currentWizard = nullptr;
            }
        }

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        hierarchy.Draw(scene);

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        propertiesPanel.Draw(hierarchy.GetSelectedEntity(), scene);

        for (std::optional<ViewportPanel>& viewport : viewports)
        {
            if (!viewport.has_value()) continue;

            ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
            viewport->Draw();
        }
    }

    #pragma clang diagnostic pop

    ViewportID Editor::CreateViewport(const ViewportCreateInfo& createInfo)
    {
        const ViewportID ID = viewportIndexPool.GenerateIndex();

        if (ID >= viewports.size()) viewports.emplace_back(createInfo);
        else viewports[ID].emplace(createInfo);

        viewportIDs.emplace_back(ID);
        return ID;
    }

    bool Editor::DestroyViewport(const ViewportID ID)
    {
        if (ID >= viewports.size() || !viewports[ID].has_value())
        {
            return false;
        }

        viewports[ID] = std::nullopt;
        viewportIndexPool.FreeIndex(ID);

        const auto iterator = std::find(viewportIDs.begin(), viewportIDs.end(), ID);
        if (iterator != viewportIDs.end()) viewportIDs.erase(iterator);

        return true;
    }

}