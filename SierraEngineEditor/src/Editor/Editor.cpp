//
// Created by Nikolay Kanchevski on 3.08.24.
//

#include "Editor.h"

#include "Panels/MenuPanel.h"
#include "Panels/ViewportPanel.h"
#include "Panels/HierarchyPanel.h"
#include "Panels/PropertiesPanel.h"

#include "Wizards/Assets/TextureSerializeWizard.h"
#include "Wizards/Assets/MaterialSerializeWizard.h"
#include "Wizards/Assets/ModelSerializeWizard.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Editor::Editor(const EditorCreateInfo& createInfo)
        : platformContext(&createInfo.platformContext), fileManager(&createInfo.fileManager), renderingContext(&createInfo.renderingContext),
          scene({ .name = "Scene" }),
          assetBrowserPanel({ .fileManager = createInfo.fileManager, .renderingContext = createInfo.renderingContext, .rootDirectoryPath = createInfo.project.GetAssetPath() }),
          logPanel({ .logger = Sierra::Logger::GetApplicationLogger() })
    {
        style = EditorThemeToImGuiStyle(createInfo.theme);

        createInfo.window.AddEventListener<Sierra::WindowFocusEvent>([this](const Sierra::WindowFocusEvent& event) -> bool
        {
            assetBrowserPanel.Refresh();
            return false;
        });
    }

    /* --- POLLING METHODS --- */

    void Editor::Draw(Sierra::CommandBuffer& commandBuffer, const Sierra::InputManager& inputManager)
    {
        ImGui::GetStyle() = style;
        const ImGuiID dockID = DrawDockSpace();

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        HierarchyPanel::Draw({ .scene = scene, .selectedEntities = selectedEntities });

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        PropertiesPanel::Draw({ .scene = scene, .selectedEntity = !selectedEntities.empty() ? &selectedEntities[0] : nullptr });

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        assetBrowserPanel.Draw({ .commandBuffer = commandBuffer, .inputManager = inputManager });

        ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
        logPanel.Draw();

        uint32 i = 0;
        viewports.ForEach([dockID, &i](Viewport& viewport) -> void
        {
            const std::string title = SR_FORMAT("Viewport [{0}]", i);
            const ViewportPanelDrawInfo drawInfo
            {
                .title = title,
                .viewport = viewport
            };

            ImGui::SetNextWindowDockID(dockID, ImGuiCond_FirstUseEver);
            ViewportPanel::Draw(drawInfo);
            i++;
        });

        if (currentWizard != nullptr && !currentWizard->Draw(commandBuffer))
        {
            currentWizard.reset();
        }
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

    void Editor::DrawMenuBar()
    {
        // TODO: Fix bug
        const std::array fileSerializeItems
        {
            MenuItem {
                .title = "Texture",
                .Callback = [this]() -> void { OpenTextureSerializeWizard(); }
            },
            MenuItem {
                .title = "Material",
                .Callback = [this]() -> void { OpenMaterialSerializeWizard(); }
            },
            MenuItem {
                .title = "Model",
                .Callback = [this]() -> void { OpenModelSerializeWizard(); }
            }
        };

        const std::array fileItems
        {
            MenuItem {
                .title = "Serialize",
                .items = fileSerializeItems
            },
            MenuItem {
                .title = "Import"
            }
        };

        const std::array items
        {
            MenuItem {
                .title = "File",
                .items = fileItems
            }
        };

        MenuPanel::Draw({ .items = items });
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
        ImGui::DockSpace(dockID, { 0.0f, 0.0f }, static_cast<ImGuiDockNodeFlags>(ImGuiDockNodeFlags_NoWindowMenuButton) | ImGuiDockNodeFlags_PassthruCentralNode);
        #pragma clang diagnostic pop
        ImGui::End();

        return dockID;
    }

    void Editor::OpenTextureSerializeWizard(const std::filesystem::path* outputDirectoryPath)
    {
        const std::vector<std::filesystem::path> filePath = platformContext->OpenFileSelectDialog({ .allowedFileExtensions = TextureSerializeWizard::INPUT_FILE_EXTENSIONS });
        if (filePath.empty()) return;

        const TextureSerializeWizardCreateInfo createInfo
        {
            .platformContext = *platformContext,
            .renderingContext = *renderingContext,
            .fileManager = *fileManager,
            .inputFilePath = &filePath.front(),
            .outputDirectoryPath = outputDirectoryPath
        };

        currentWizard = std::make_unique<TextureSerializeWizard>(createInfo);
    }

    void Editor::OpenMaterialSerializeWizard(const std::filesystem::path* outputDirectoryPath)
    {
        const MaterialSerializeWizardCreateInfo createInfo
        {
            .platformContext = *platformContext,
            .fileManager = *fileManager,
            .outputDirectoryPath = outputDirectoryPath
        };

        currentWizard = std::make_unique<MaterialSerializeWizard>(createInfo);
    }

    void Editor::OpenModelSerializeWizard(const std::filesystem::path* outputDirectoryPath)
    {
        const std::vector<std::filesystem::path> filePath = platformContext->OpenFileSelectDialog({ .allowedFileExtensions = ModelSerializeWizard::INPUT_FILE_EXTENSIONS });
        if (filePath.empty()) return;

        const ModelSerializeWizardCreateInfo createInfo
        {
            .platformContext = *platformContext,
            .renderingContext = *renderingContext,
            .fileManager = *fileManager,
            .inputFilePath = &filePath.front(),
            .outputDirectoryPath = outputDirectoryPath
        };

        currentWizard = std::make_unique<ModelSerializeWizard>(createInfo);
    }


}