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
            .outputFileExtension = OUTPUT_FILE_EXTENSION,
            .outputDirectoryPath = createInfo.outputDirectoryPath
          })
    {
        AssetMetadata& metadata = serializeInfo.metadata;
        metadata.author = createInfo.platformContext.GetUserName();
        metadata.name = "Material";
    }

    /* --- POLLING METHODS --- */

    bool MaterialSerializeWizard::Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame)
    {
        bool open = BeginWizard("Material Serialize Wizard");
        if (open)
        {
            DrawMetadataMenu(serializeInfo.metadata);

            DrawSettingsMenu();

            constexpr std::array OPTIONS
            {
                ImGuiDropdownOption { .text = "YAML" }
            };

            if (DrawFileOutputFooter(applicationFrame.platformContext, materialFormatIndex, OPTIONS))
            {
                if (Serialize(applicationFrame))
                {
                    open = false;
                }
            }

            EndWizard();
        }

        return open;
    }

    void MaterialSerializeWizard::DrawSettingsMenu() noexcept
    {
        if (ImGui::TreeNodeEx("Settings", MENU_TREE_FLAGS))
        {
            if (ImGui::TreeNodeEx("Diffuse", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding ))
            {
                if (ImGuiWidgets::BeginPropertyTable())
                {
                    ImGuiWidgets::BeginProperty("Tint");
                    {
                        ImGuiWidgets::ColorInput("##DiffuseColorInput", serializeInfo.settings.diffuse.tint);
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
                        ImGuiWidgets::NumericInput("##SpecularShininessInput", serializeInfo.settings.specular.shininess, 0.0f, 512.0f);
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

                    if (uint32 alphaModeIndex = static_cast<uint32>(serializeInfo.settings.alphaMode); ImGuiWidgets::Dropdown("##CullModeDropdown", alphaModeIndex, { .options = OPTIONS }))
                    {
                        serializeInfo.settings.alphaMode = static_cast<AlphaMode>(alphaModeIndex);
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

                    if (uint32 cullModeIndex = static_cast<uint32>(serializeInfo.settings.cullMode); ImGuiWidgets::Dropdown("##CullModeDropdown", cullModeIndex, { .options = OPTIONS }))
                    {
                        serializeInfo.settings.cullMode = static_cast<CullMode>(cullModeIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

    bool MaterialSerializeWizard::Serialize(const ApplicationFrame& applicationFrame) noexcept
    {
        const MaterialSerializeFormat format = static_cast<MaterialSerializeFormat>(materialFormatIndex);
        std::unique_ptr<MaterialSerializer> serializer = nullptr;

        switch (format)
        {
            case MaterialSerializeFormat::YAML: { serializer = std::make_unique<YAMLMaterialSerializer>(); break; }
        }

        MaterialID ID = { };
        const std::optional<SerializedMaterial> serializedMaterial = serializer->Serialize(serializeInfo, ID);

        if (!serializedMaterial.has_value())
        {
            APP_WARNING("Could not serialize material [{0}]", serializeInfo.metadata.name);
            return false;
        }

        const Sierra::Path& outputDataFilePath = GetOutputFilePath();
        if (outputDataFilePath.IsEmpty())
        {
            APP_WARNING("Cannot serialize material [{0}] at invalid location [{1}]", serializeInfo.metadata.name, outputDataFilePath.GetString());
            return false;
        }

        if (!WriteAssetFiles(applicationFrame.platformContext.GetFileManager(), outputDataFilePath, *serializedMaterial))
        {
            APP_WARNING("Failed to write serialized material to file storage");
            return false;
        }

        return true;
    }

}