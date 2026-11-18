//
// Created by Nikolay Kanchevski on 22.10.25.
//

#include "FileOutputWizard.h"

namespace SierraEngine
{

    namespace
    {
        constexpr ImGuiPropertyTableInfo PROPERTY_TABLE_INFO
        {
            .tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInner | ImGuiTableFlags_PadOuterX
        };
    }

    /* --- POLLING METHODS --- */

    FileOutputWizard::FileOutputWizard(const FileOutputWizardCreateInfo& createInfo) noexcept
        : outputFileExtension(createInfo.outputFileExtension)
    {
        if (createInfo.inputFilePath == nullptr) return;

        if (createInfo.outputDirectoryPath != nullptr)
        {
            outputFilePath = *createInfo.outputDirectoryPath / Sierra::Path(createInfo.inputFilePath->GetFileName()).ChangeExtension(outputFileExtension);
        }
        else
        {
            outputFilePath = Sierra::Path(*createInfo.inputFilePath).ChangeExtension(outputFileExtension);
        }
    }

    bool FileOutputWizard::DrawFileOutputFooter(const Sierra::PlatformContext& platformContext, uint32& formatIndex, const std::span<const ImGuiDropdownOption> formatOptions) noexcept
    {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float32 buttonHeight = ImGui::GetFrameHeight() * 1.35f;

        if (ImGuiWidgets::BeginPropertyTable(PROPERTY_TABLE_INFO))
        {
            ImGuiWidgets::BeginProperty("Format");
            {
                ImGuiWidgets::Dropdown("##FormatDropdown", formatIndex, { .options = formatOptions });
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::BeginProperty("Output");
            {
                const std::string fileName = outputFilePath.GetStem();
                const std::array allowedExtensions = { outputFileExtension };

                const ImGuiFileSaveInputInfo inputInfo
                {
                    .directoryPath = &outputFilePath,
                    .fileName = fileName,
                    .allowedFileExtensions = allowedExtensions
                };

                if (ImGuiWidgets::FileSaveInput("##OutputFileSaveInput", outputFilePath, platformContext, inputInfo))
                {
                    SetOutputFilePath(outputFilePath);
                }
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::EndPropertyTable();
        }

        const float32 buttonWidth = ImGui::GetContentRegionAvail().x;
        return ImGui::Button("Serialize", { buttonWidth, buttonHeight });
    }

    /* --- SETTER METHODS --- */

    void FileOutputWizard::SetOutputFilePath(const Sierra::Path& givenOutputFilePath) noexcept
    {
        outputFilePath = givenOutputFilePath;
    }

}