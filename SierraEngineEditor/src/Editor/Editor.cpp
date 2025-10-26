//
// Created by Nikolay Kanchevski on 3.08.24.
//

#include "Editor.h"

#include "Wizards/Assets/TextureSerializeWizard.h"
#include "Wizards/Assets/MaterialSerializeWizard.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Editor::Editor(const EditorCreateInfo& createInfo)
        : device(createInfo.device), platformContext(createInfo.platformContext), resourceTable(createInfo.resourceTable), scene(createInfo.scene)
    {
        style = EditorThemeToImGuiStyle(createInfo.theme);
    }

    /* --- POLLING METHODS --- */

    void Editor::Draw(Sierra::CommandBuffer& commandBuffer)
    {
        ImGui::GetStyle() = style;
        const ImGuiID dockID = DrawDockSpace();

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

        if (currentWizard != nullptr)
        {
            bool open = true;
            currentWizard->Draw(open, commandBuffer, resourceTable);

            if (!open)
            {
                currentWizard = nullptr;
            }
        }
    }

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

    void Editor::DrawMenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::BeginMenu("Serialize"))
                {
                    if (ImGui::MenuItem("Texture"))
                    {
                        const std::optional<std::filesystem::path> filePath = platformContext.OpenSingleFileSelectDialog({ .allowedFileExtensions = TextureSerializeWizard::INPUT_FILE_EXTENSIONS });
                        if (filePath.has_value())
                        {
                            const TextureSerializeWizardCreateInfo createInfo
                            {
                                .platformContext = platformContext,
                                .device = device,
                                .inputFilePath = &filePath.value()
                            };

                            currentWizard = std::make_unique<TextureSerializeWizard>(createInfo);
                        }
                    }
                    else if (ImGui::MenuItem("Material"))
                    {
                        const MaterialSerializeWizardCreateInfo createInfo
                        {
                            .platformContext = platformContext,
                            .device = device
                        };

                        currentWizard = std::make_unique<MaterialSerializeWizard>(createInfo);
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Import"))
                {
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    ImGuiID Editor::DrawDockSpace()
    {
        const ImGuiID dockID = ImGui::GetID("Dockspace");

        const ImGuiViewport* const viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

        // Create docked viewport
        ImGui::Begin("##BackgroundViewport", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_MenuBar);
        ImGui::PopStyleVar(2);

        DrawMenuBar();

        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
        ImGui::DockSpace(dockID, { 0.0f, 0.0f }, ImGuiDockNodeFlags_NoWindowMenuButton | ImGuiDockNodeFlags_PassthruCentralNode);
        #pragma clang diagnostic pop
        ImGui::End();

        return dockID;
    }

}