//
// Created by Nikolay Kanchevski on 4.11.25.
//

#include "MenuPanel.h"

#include "../Wizards/Assets/MaterialSerializeWizard.h"
#include "../Wizards/Assets/TextureSerializeWizard.h"
#include "../Wizards/Assets/ModelSerializeWizard.h"

namespace SierraEngine
{

    namespace
    {
        void SerializeMaterial(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
        {
            const Sierra::Path assetsDirectoryPath = applicationFrame.project.GetAssetPath();

            const MaterialSerializeWizardCreateInfo createInfo
            {
                .platformContext = applicationFrame.platformContext,
                .outputDirectoryPath = &assetsDirectoryPath
            };

            std::unique_ptr<Modal> modal = std::make_unique<MaterialSerializeWizard>(createInfo);
            editorFrame.modalManager.PushModal(std::move(modal));
        }

        void SerializeTexture(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
        {
            const std::vector<Sierra::Path> filePath = applicationFrame.platformContext.OpenPathSelectionDialog({ .allowedFileExtensions = TextureSerializeWizard::INPUT_FILE_EXTENSIONS });
            if (filePath.empty()) return;

            const Sierra::Path assetsDirectoryPath = applicationFrame.project.GetAssetPath();

            const TextureSerializeWizardCreateInfo createInfo
            {
                .platformContext = applicationFrame.platformContext,
                .inputFilePath = &filePath.front(),
                .outputDirectoryPath = &assetsDirectoryPath
            };

            std::unique_ptr<Modal> modal = std::make_unique<TextureSerializeWizard>(createInfo);
            editorFrame.modalManager.PushModal(std::move(modal));
        }

        void SerializeModel(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
        {
            const std::vector<Sierra::Path> filePath = applicationFrame.platformContext.OpenPathSelectionDialog({ .allowedFileExtensions = ModelSerializeWizard::INPUT_FILE_EXTENSIONS });
            if (filePath.empty()) return;

            // TODO: Figure out a way to pass path views
            const Sierra::Path assetsDirectoryPath = applicationFrame.project.GetAssetPath();

            const ModelSerializeWizardCreateInfo createInfo
            {
                .platformContext = applicationFrame.platformContext,
                .inputFilePath = &filePath.front(),
                .outputDirectoryPath = &assetsDirectoryPath
            };

            std::unique_ptr<Modal> modal = std::make_unique<ModelSerializeWizard>(createInfo);
            editorFrame.modalManager.PushModal(std::move(modal));
        }

        void DrawFileMenu(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
        {
            if (!ImGui::BeginMenu("File"))
                return;

            if (ImGui::BeginMenu("Serialize"))
            {
                if (ImGui::MenuItem("Texture"))
                {
                    SerializeTexture(applicationFrame, editorFrame);
                }
                if (ImGui::MenuItem("Material"))
                {
                    SerializeMaterial(applicationFrame, editorFrame);
                }
                if (ImGui::MenuItem("Model"))
                {
                    SerializeModel(applicationFrame, editorFrame);
                }

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Import"))
            {
                ImGui::EndMenu();
            }

            ImGui::EndMenu();
        }
    }

    /* --- POLLING METHODS --- */

    void MenuPanel::Draw(const ApplicationFrame& applicationFrame, const EditorFrame& editorFrame)
    {
        if (ImGui::BeginMenuBar())
        {
            DrawFileMenu(applicationFrame, editorFrame);
            ImGui::EndMenuBar();
        }
    }

}