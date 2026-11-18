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
            .inputFileExtensions = createInfo.inputFileExtensions,
            .inputFilePath = createInfo.inputFilePath,
            .outputFileExtension = createInfo.outputFileExtension,
            .outputDirectoryPath = createInfo.outputDirectoryPath
        })
    {

    }

    /* --- POLLING METHODS --- */

    void AssetSerializeWizard::DefaultMetadata(AssetMetadata& metadata, const std::string_view name, const Sierra::PlatformContext& platformContext) const noexcept
    {
        metadata.name = name;
        metadata.author = platformContext.GetUserName();
        metadata.tags = { };
    }

    bool AssetSerializeWizard::WriteAssetFiles(const Sierra::FileManager& fileManager, const Sierra::Path& assetFilePath, const SerializedAsset& serializedAsset) const
    {
        if (fileManager.WriteFile(assetFilePath, serializedAsset.data, Sierra::PathConflictPolicy::Overwrite).has_value())
            return false;

        const Sierra::Path blobFilePath = Sierra::Path(assetFilePath).ChangeExtension(BLOB_OUTPUT_FILE_EXTENSION);
        if (fileManager.WriteFile(blobFilePath, serializedAsset.blob, Sierra::PathConflictPolicy::Overwrite).has_value())
            return false;

        return true;
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
