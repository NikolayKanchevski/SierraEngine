//
// Created by Nikolay Kanchevski on 22.10.25.
//

#include "FileOutputWizard.h"

#include "../Widgets/ImGuiWidgets.h"

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
        : platformContext(&createInfo.platformContext),
          outputFileExtension(createInfo.outputFileExtension)
    {
        if (createInfo.inputFilePath == nullptr) return;

        if (createInfo.outputDirectoryPath != nullptr)
        {
            outputFilePath = *createInfo.outputDirectoryPath / createInfo.inputFilePath->filename().replace_extension(outputFileExtension);
        }
        else
        {
            outputFilePath = std::filesystem::path(*createInfo.inputFilePath).replace_extension(outputFileExtension);
        }
    }

    bool FileOutputWizard::DrawFileOutputFooter(uint32& formatIndex, std::span<const ImGuiDropdownOption> formatOptions) noexcept
    {
        const ImGuiStyle& style = ImGui::GetStyle();
        const float32 buttonHeight = ImGui::GetFrameHeight() * 1.35f;

        const float32 stickyBottomIndent = style.FramePadding.y + buttonHeight + style.ItemSpacing.y + 2 * (2 * style.FramePadding.y + ImGui::GetFrameHeight() + style.CellPadding.y);
        if (const float32 availableVerticalSpace = ImGui::GetContentRegionAvail().y; stickyBottomIndent < availableVerticalSpace)
        {
            ImGuiWidgets::VerticalIndent(availableVerticalSpace - stickyBottomIndent);
        }

        if (ImGuiWidgets::BeginPropertyTable(PROPERTY_TABLE_INFO))
        {
            ImGuiWidgets::BeginProperty("Format");
            {
                ImGuiWidgets::Dropdown("##FormatDropdown", formatIndex, { .options = formatOptions });
            }
            ImGuiWidgets::EndProperty();

            ImGuiWidgets::BeginProperty("Output");
            {
                const std::string fileName = outputFilePath.stem().string();
                const std::array allowedExtensions = { outputFileExtension };

                const ImGuiFileSaveInputInfo inputInfo
                {
                    .directoryPath = &outputFilePath,
                    .fileName = fileName,
                    .allowedFileExtensions = allowedExtensions
                };

                if (ImGuiWidgets::FileSaveInput("##OutputFileSaveInput", outputFilePath, *platformContext, inputInfo))
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

    void FileOutputWizard::SetOutputFilePath(const std::filesystem::path& givenOutputFilePath) noexcept
    {
        outputFilePath = givenOutputFilePath;
    }

}