//
// Created by Nikolay Kanchevski on 24.10.25.
//

#include "MaterialSerializeWizard.h"

#include "../../../Assets/Materials/Serializers/YAMLMaterialSerializer.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    MaterialSerializeWizard::MaterialSerializeWizard(const MaterialSerializeWizardCreateInfo& createInfo) noexcept
        : AssetSerializeWizard({
            .platformContext = createInfo.platformContext,
            .outputFileExtension = OUTPUT_FILE_EXTENSION,
            .outputDirectoryPath = createInfo.outputDirectoryPath
          }),
          device(createInfo.device)
    {
        DefaultMetadata(serializeInfo.metadata, "Material");
    }

    /* --- POLLING METHODS --- */

    void MaterialSerializeWizard::Draw(bool& open, Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            open = false;
            return;
        }

        if (BeginWizard("Texture Serialize Wizard", open))
        {
            DrawMetadataMenu(serializeInfo.metadata);

            DrawPropertiesMenu();

            constexpr std::array OPTIONS
            {
                ImGuiDropdownOption { .text = "YAML" }
            };

            if (DrawFileOutputFooter(materialFormatIndex, OPTIONS))
            {
                if (Serialize())
                {
                    open = false;
                }
            }

            EndWizard();
        }
    }

    void MaterialSerializeWizard::DrawPropertiesMenu() noexcept
    {
        if (ImGui::TreeNodeEx("Properties", MENU_TREE_FLAGS))
        {
            if (ImGui::TreeNodeEx("Diffuse", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding ))
            {
                if (ImGuiWidgets::BeginPropertyTable())
                {
                    ImGuiWidgets::BeginProperty("Tint");
                    {
                        ImGuiWidgets::ColorInput("##DiffuseColorInput", serializeInfo.properties.diffuse.tint);
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::BeginProperty("Texture");
                    {
                        static uint64 ID;
                        ImGuiWidgets::NumericInput("##DiffuseTextureInput", ID);
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::EndPropertyTable();
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNodeEx("Specular", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding ))
            {
                if (ImGuiWidgets::BeginPropertyTable())
                {
                    ImGuiWidgets::BeginProperty("Shininess");
                    {
                        ImGuiWidgets::NumericInput("##SpecularShininessInput", serializeInfo.properties.specular.shininess, 0.0f, 512.0f);
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::BeginProperty("Texture");
                    {
                        static uint64 ID;
                        ImGuiWidgets::NumericInput("##SpecularTextureInput", ID);
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::EndPropertyTable();
                }

                ImGui::TreePop();
            }

            if (ImGuiWidgets::BeginPropertyTable())
            {
                ImGuiWidgets::BeginProperty("Alpha Mode");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "Opaque" },
                        ImGuiDropdownOption { .text = "Transparent" }
                    };

                    if (uint32 alphaModeIndex = static_cast<uint32>(serializeInfo.properties.alphaMode); ImGuiWidgets::Dropdown("##CullModeDropdown", alphaModeIndex, { .options = OPTIONS }))
                    {
                        serializeInfo.properties.alphaMode = static_cast<MaterialAlphaMode>(alphaModeIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Cull Mode");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "Single Sided" },
                        ImGuiDropdownOption { .text = "Double Sided" }
                    };

                    if (uint32 cullModeIndex = static_cast<uint32>(serializeInfo.properties.cullMode); ImGuiWidgets::Dropdown("##CullModeDropdown", cullModeIndex, { .options = OPTIONS }))
                    {
                        serializeInfo.properties.cullMode = static_cast<MaterialCullMode>(cullModeIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

    bool MaterialSerializeWizard::Serialize() noexcept
    {
        const MaterialSerializeFormat format = static_cast<MaterialSerializeFormat>(materialFormatIndex);
        std::unique_ptr<MaterialSerializer> serializer = nullptr;

        switch (format)
        {
            case MaterialSerializeFormat::YAML: { serializer = std::make_unique<YAMLMaterialSerializer>(); break; }
        }

        const std::optional<SerializedMaterial> serializedMaterial = serializer->Serialize(serializeInfo);
        if (!serializedMaterial.has_value())
        {
            APP_WARNING("Could not serialize material [{0}]", serializeInfo.metadata.name);
            return false;
        }

        const Sierra::FileManager& fileManager = FileOutputWizard::GetPlatformContext().GetFileManager();

        const std::filesystem::path& outputDataFilePath = GetOutputFilePath();
        fileManager.CreateFile(outputDataFilePath, Sierra::FilePathConflictPolicy::Overwrite);
        fileManager.WriteFile(outputDataFilePath, serializedMaterial->data);

        const std::filesystem::path outputBlobFilePath = std::filesystem::path(outputDataFilePath).replace_extension(ASSET_OUTPUT_FILE_BLOB_EXTENSION);
        fileManager.CreateFile(outputBlobFilePath, Sierra::FilePathConflictPolicy::Overwrite);
        fileManager.WriteFile(outputBlobFilePath, serializedMaterial->blob);

        return true;
    }

}