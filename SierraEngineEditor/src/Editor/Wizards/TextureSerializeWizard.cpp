//
// Created by Nikolay Kanchevski on 7.11.24.
//

#include "TextureSerializeWizard.h"

#include "../Widgets/ImGuiWidgets.h"

namespace SierraEngine
{

    namespace
    {
        constexpr std::array<std::string_view, 8> TEXTURE_FILE_EXTENSIONS = { ".jpg", ".jpeg", ".png", ".bmp", ".psd", ".tga", ".gif", ".hdr" };
        constexpr std::string_view TEXTURE_ASSET_EXTENSION = ".texture";
    }

    /* --- CONSTRUCTORS --- */

    TextureSerializeWizard::TextureSerializeWizard(const TextureSerializeWizardCreateInfo& createInfo)
        : platformContext(&createInfo.platformContext), sourceFilePath(createInfo.selectedFilePath)
    {
        if (!createInfo.selectedFilePath.empty()) ApplyDefaults();
    }

    /* --- POLLING METHODS --- */

    void TextureSerializeWizard::Draw(bool& open)
    {
        if ((sourceFilePath.empty() && !SelectSourceFile()) || ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            open = false;
            return;
        }

        DrawShadow();

        const ImGuiIO& io = ImGui::GetIO();
        const Vector2 windowSize = { io.DisplaySize.x / 2.75f, io.DisplaySize.y / 1.25f };

        ImGui::SetNextWindowSize(windowSize);
        ImGui::SetNextWindowPos({ (io.DisplaySize.x - windowSize.x) / 2.0f, (io.DisplaySize.y - windowSize.y) / 2.0f });
        if (ImGui::Begin("Texture Serialize Wizard", &open, DEFAULT_WINDOW_FLAGS))
        {
            const ImGuiStyle& style = ImGui::GetStyle();
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, style.CellPadding * 2);

            const float32 indent = ImGui::GetContentRegionAvail().x / 30.0f;

            ImGuiWidgets::PushID("TextureSerializeWizardHeader");
            {
                ImGuiWidgets::BeginHorizontalPadding(indent);
                ImGuiWidgets::BeginPropertyTable(DEFAULT_HEADER_TABLE_FLAGS);

                ImGuiWidgets::BeginProperty("Source");
                {
                    if (ImGuiWidgets::FilePathInput("##SourceFilePathInput", sourceFilePath))
                    {
                        SelectSourceFile();
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::EndPropertyTable();
                ImGuiWidgets::EndPadding();
            }
            ImGuiWidgets::PopID();

            ImGuiWidgets::PushID("TextureSerializeWizardMetadata");
            if (ImGui::TreeNodeEx("Metadata", DEFAULT_BODY_TREE_FLAGS))
            {
                ImGuiWidgets::BeginHorizontalPadding();
                ImGuiWidgets::BeginPropertyTable(DEFAULT_BODY_TABLE_FLAGS);

                ImGuiWidgets::BeginProperty("Name");
                {
                    ImGui::InputText("##NameInput", &serializeInfo.metadata.name);
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Author");
                {
                    ImGui::InputText("##AuthorInput", &serializeInfo.metadata.author);
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::EndPropertyTable();
                ImGuiWidgets::EndPadding();
            }
            ImGuiWidgets::PopID();

            ImGuiWidgets::PushID("TextureSerializeWizardSerializeInfo");
            if (ImGui::TreeNodeEx("Serialize Info", DEFAULT_BODY_TREE_FLAGS))
            {
                ImGuiWidgets::BeginHorizontalPadding();
                ImGuiWidgets::BeginPropertyTable(DEFAULT_BODY_TABLE_FLAGS);

                ImGuiWidgets::BeginProperty("Format");
                {
                    constexpr std::array<ImGuiDropdownOption, 1> options
                    {
                        ImGuiDropdownOption { .text = "YAML" }
                    };

                    if (uint32 formatIndex = 0; ImGuiWidgets::Dropdown("##FormatDropdown", &formatIndex, { .options = options }))
                    {

                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Type");
                {
                    constexpr std::array<ImGuiDropdownOption, 12> options
                    {
                        ImGuiDropdownOption { .text = "Undefined" },
                        ImGuiDropdownOption { .text = "Albedo" },
                        ImGuiDropdownOption { .text = "Specular" },
                        ImGuiDropdownOption { .text = "Normal" },
                        ImGuiDropdownOption { .text = "Opacity" },
                        ImGuiDropdownOption { .text = "Roughness" },
                        ImGuiDropdownOption { .text = "Metallic" },
                        ImGuiDropdownOption { .text = "Displacement" },
                        ImGuiDropdownOption { .text = "Emission" },
                        ImGuiDropdownOption { .text = "Occlusion" },
                        ImGuiDropdownOption { .text = "Shadow" },
                        ImGuiDropdownOption { .text = "Environment" }
                    };

                    if (uint32 typeIndex = static_cast<uint32>(serializeInfo.type); ImGuiWidgets::Dropdown("##TypeDropdown", &typeIndex, { .options = options }))
                    {
                        serializeInfo.type = static_cast<TextureType>(typeIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Filtering");
                {
                    constexpr std::array<ImGuiDropdownOption, 2> options
                    {
                        ImGuiDropdownOption { .text = "Pixelated" },
                        ImGuiDropdownOption { .text = "Smooth" }
                    };

                    if (uint32 filterIndex = static_cast<uint32>(serializeInfo.filter); ImGuiWidgets::Dropdown("##FilterDropdown", &filterIndex, { .options = options }))
                    {
                        serializeInfo.filter = static_cast<Sierra::SamplerFilter>(filterIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Compression");
                {
                    constexpr std::array<ImGuiDropdownOption, 2> options
                    {
                        ImGuiDropdownOption { .text = "None" },
                        ImGuiDropdownOption { .text = "Basis Universal" }
                    };

                    if (uint32 compressorTypeIndex = static_cast<uint32>(serializeInfo.compressorType); ImGuiWidgets::Dropdown("##CompressorTypeDropdown", &compressorTypeIndex, { .options = options }))
                    {
                        serializeInfo.compressorType = static_cast<ImageCompressorType>(compressorTypeIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                if (serializeInfo.compressorType != ImageCompressorType::Undefined)
                {
                    ImGuiWidgets::BeginProperty("Compression Level");
                    {
                        constexpr std::array<ImGuiDropdownOption, 5> options
                        {
                            ImGuiDropdownOption { .text = "Lowest" },
                            ImGuiDropdownOption { .text = "Low" },
                            ImGuiDropdownOption { .text = "Standard" },
                            ImGuiDropdownOption { .text = "High" },
                            ImGuiDropdownOption { .text = "Highest" }
                        };

                        if (uint32 compressionLevelIndex = static_cast<uint32>(serializeInfo.compressionLevel); ImGuiWidgets::Dropdown("##CompressionLevelDropdown", &compressionLevelIndex, { .options = options }))
                        {
                            serializeInfo.compressionLevel = static_cast<ImageCompressionLevel>(compressionLevelIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::BeginProperty("Quality Level");
                    {
                        constexpr std::array<ImGuiDropdownOption, 5> options
                        {
                            ImGuiDropdownOption { .text = "Lowest" },
                            ImGuiDropdownOption { .text = "Low" },
                            ImGuiDropdownOption { .text = "Standard" },
                            ImGuiDropdownOption { .text = "High" },
                            ImGuiDropdownOption { .text = "Highest" }
                        };

                        if (uint32 qualityLevelIndex = static_cast<uint32>(serializeInfo.qualityLevel); ImGuiWidgets::Dropdown("##QualityLevelDropdown", &qualityLevelIndex, { .options = options }))
                        {
                            serializeInfo.qualityLevel = static_cast<ImageQualityLevel>(qualityLevelIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();
                }

                ImGuiWidgets::EndPropertyTable();
                ImGuiWidgets::EndPadding();
            }
            ImGuiWidgets::PopID();

            ImGuiWidgets::PushID("TextureSerializeWizardFooter");
            {
                const float32 buttonHeight = ImGui::GetFrameHeight();
                ImGui::Dummy({ 0, ImGui::GetContentRegionAvail().y - (indent + buttonHeight + style.ItemSpacing.y + ImGui::GetFrameHeight() + 2 * style.CellPadding.y) });

                ImGuiWidgets::BeginHorizontalPadding(indent);

                ImGuiWidgets::BeginPropertyTable(DEFAULT_HEADER_TABLE_FLAGS);
                ImGuiWidgets::BeginProperty("Output");
                {
                    if (ImGuiWidgets::FilePathInput("##OutputFilePathInput", outputFilePath))
                    {
                        SelectOutputFile();
                    }
                }
                ImGuiWidgets::EndProperty();
                ImGuiWidgets::EndPropertyTable();

                const float32 buttonWidth = ImGui::GetContentRegionAvail().x;
                ImGui::Button("Serialize", { buttonWidth, buttonHeight });

                ImGuiWidgets::EndPadding();
            }
            ImGuiWidgets::PopID();

            ImGui::PopStyleVar();
        }
        ImGui::End();
    }

    /* --- POLLING METHODS --- */

    bool TextureSerializeWizard::SelectSourceFile()
    {
        std::vector<std::filesystem::path> sourceFilePaths = platformContext->OpenFileSelectDialog({ .allowedFileExtensions = TEXTURE_FILE_EXTENSIONS });
        if (sourceFilePaths.empty()) return false;

        sourceFilePath = std::move(sourceFilePaths.front());
        ApplyDefaults();

        return true;
    }

    void TextureSerializeWizard::ApplyDefaults()
    {
        outputFilePath = std::filesystem::path(sourceFilePath).replace_extension(TEXTURE_ASSET_EXTENSION);

        serializeInfo.metadata.author = std::string(platformContext->GetUserName().data(), platformContext->GetUserName().size());
        serializeInfo.metadata.author[0] = static_cast<char>(std::toupper(serializeInfo.metadata.author[0]));
        serializeInfo.metadata.name = sourceFilePath.stem().string();

        serializeInfo.type = TextureType::Albedo;
        serializeInfo.filter = Sierra::SamplerFilter::Linear;
    }

    bool TextureSerializeWizard::SelectOutputFile()
    {
        std::optional<std::filesystem::path> path = platformContext->OpenFileSaveDialog({ .directoryPath = sourceFilePath.parent_path(), .fileName = outputFilePath.stem().string(), .allowedFileExtensions = {{ TEXTURE_ASSET_EXTENSION }} });
        if (!path.has_value()) return false;

        outputFilePath = std::move(*path);
        return true;
    }

}