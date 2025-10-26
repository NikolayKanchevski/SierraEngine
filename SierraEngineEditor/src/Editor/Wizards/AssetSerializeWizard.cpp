//
// Created by Nikolay Kanchevski on 10.10.25.
//

#include "AssetSerializeWizard.h"

namespace SierraEngine
{

    namespace
    {
        constexpr ImGuiPropertyTableInfo PROPERTY_TABLE_INFO
        {
            .tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInner | ImGuiTableFlags_PadOuterX
        };
    }

    /* --- CONSTRUCTORS --- */

    AssetSerializeWizard::AssetSerializeWizard(const AssetSerializeWizardCreateInfo& createInfo) noexcept
        : FileOutputWizard({
            .platformContext = createInfo.platformContext,
            .inputFileExtensions = createInfo.inputFileExtensions,
            .inputFilePath = createInfo.inputFilePath,
            .outputFileExtension = createInfo.outputFileExtension,
            .outputDirectoryPath = createInfo.outputDirectoryPath
        })
    {

    }

    /* --- POLLING METHODS --- */

    void AssetSerializeWizard::DefaultMetadata(AssetMetadata& metadata, const std::string_view name) const noexcept
    {
        metadata.name = name;
        metadata.author = GetPlatformContext().GetUserName();
        metadata.tags = { };
    }

    void AssetSerializeWizard::DrawMetadataMenu(AssetMetadata& metadata) const noexcept
    {
        if (ImGui::TreeNodeEx("Metadata", MENU_TREE_FLAGS))
        {
            if (ImGuiWidgets::BeginPropertyTable())
            {
                ImGuiWidgets::BeginProperty("Name");
                {
                    ImGuiWidgets::TextInput("##NameInput", metadata.name);
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Author");
                {
                    ImGuiWidgets::TextInput("##AuthorInput", metadata.author);
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Tags");
                {
                    const std::function ItemCallback = [](std::string& tag, const uint32) -> void
                    {
                        ImGuiWidgets::TextInput("##TagsItemInput", tag);
                    };

                    ImGuiWidgets::ListInput("##TagsInput", metadata.tags, { .ItemCallback = ItemCallback });
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

}
