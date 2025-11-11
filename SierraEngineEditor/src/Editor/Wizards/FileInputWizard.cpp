//
// Created by Nikolay Kanchevski on 22.10.25.
//

#include "FileInputWizard.h"

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

    FileInputWizard::FileInputWizard(const FileInputWizardCreateInfo& createInfo) noexcept
        : platformContext(&createInfo.platformContext),
          inputFileExtensions(createInfo.inputFileExtensions.begin(), createInfo.inputFileExtensions.end())
    {
        if (createInfo.inputFilePath == nullptr) return;
        inputFilePath = *createInfo.inputFilePath;
    }

    void FileInputWizard::DrawFileInputHeader() noexcept
    {
        if (ImGuiWidgets::BeginPropertyTable(PROPERTY_TABLE_INFO))
        {
            ImGuiWidgets::BeginProperty("Source");
            {
                const ImGuiFileSelectInputInfo inputInfo
                {
                    .allowedFileExtensions = inputFileExtensions
                };

                if (ImGuiWidgets::FileSelectInput("##SourceFileSelectInput", inputFilePath, *platformContext, inputInfo))
                {
                    SetInputFilePath(inputFilePath);
                }
            }
            ImGuiWidgets::EndProperty();
        }
        ImGuiWidgets::EndPropertyTable();
    }

    /* --- SETTER METHODS --- */

    void FileInputWizard::SetInputFilePath(const std::filesystem::path& givenInputFilePath) noexcept
    {
        inputFilePath = givenInputFilePath;
    }

}