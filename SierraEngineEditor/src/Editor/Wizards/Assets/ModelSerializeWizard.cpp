//
// Created by Nikolay Kanchevski on 11.11.26.
//

#include "ModelSerializeWizard.h"

#include <unordered_set>

#include "MaterialSerializeWizard.h"
#include "TextureSerializeWizard.h"
#include "../../../Assets/Models/Loaders/AutoModelLoader.h"
#include "../../../Assets/Models/Optimizers/ZeuxMeshOptimizer.h"
#include "../../../Assets/Models/Serializers/YAMLModelSerializer.h"

namespace SierraEngine
{

    namespace
    {
        [[nodiscard]] std::optional<LoadedModel> LoadModel(const Sierra::FileManager& fileManager, const std::filesystem::path& filePath) noexcept
        {
            const std::vector<uint8> memory = fileManager.ReadFile(filePath);
            if (memory.empty())
            {
                APP_WARNING("Failed to read model file [{0}]", filePath.string());
                return std::nullopt;
            }

            std::optional<LoadedModel> model = AutoModelLoader::Load({ .memory = memory });
            if (!model.has_value())
            {
                APP_WARNING("Failed to load model [{0}]", filePath.string());
                return std::nullopt;
            }

            return model;
        }

        void WriteAssetFiles(const Sierra::FileManager& fileManager, const std::filesystem::path& dataFilePath, const std::string_view blobFileExtension, const std::span<const uint8> data, const std::span<const uint8> blob)
        {
            fileManager.CreateFile(dataFilePath, Sierra::FilePathConflictPolicy::Overwrite);
            fileManager.WriteFile(dataFilePath, data);

            const std::filesystem::path blobFilePath = std::filesystem::path(dataFilePath).replace_extension(blobFileExtension);
            fileManager.CreateFile(blobFilePath, Sierra::FilePathConflictPolicy::Overwrite);
            fileManager.WriteFile(blobFilePath, blob);
        }
    }

    /* --- CONSTRUCTORS --- */

    ModelSerializeWizard::ModelSerializeWizard(const ModelSerializeWizardCreateInfo& createInfo)
        : FileInputWizard({
            .inputFileExtensions = INPUT_FILE_EXTENSIONS,
            .inputFilePath = createInfo.inputFilePath,
            .outputFileExtension = OUTPUT_FILE_EXTENSION,
            .outputDirectoryPath = createInfo.outputDirectoryPath
          }),
          AssetSerializeWizard({
            .inputFileExtensions = INPUT_FILE_EXTENSIONS,
            .inputFilePath = createInfo.inputFilePath,
            .outputFileExtension = OUTPUT_FILE_EXTENSION,
            .outputDirectoryPath = createInfo.outputDirectoryPath
          }),
          platformContext(createInfo.platformContext),
          fileManager(createInfo.fileManager),
          renderingContext(&createInfo.renderingContext)
    {
        if (createInfo.inputFilePath != nullptr)
        {
            SetInputFilePath(*createInfo.inputFilePath);
        }
    }

    /* --- POLLING METHODS --- */

    void ModelSerializeWizard::Draw(bool& open, Sierra::CommandBuffer&)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            open = false;
            return;
        }

        if (BeginWizard("Model Serialize Wizard", open))
        {
            DrawFileInputHeader(platformContext);
            DrawMetadataMenu(metadata);
            DrawSerializeInfoMenu();

            constexpr std::array OPTIONS
            {
                ImGuiDropdownOption { .text = "YAML" }
            };

            if (DrawFileOutputFooter(platformContext, modelFormatIndex, OPTIONS))
            {
                if (Serialize())
                {
                    open = false;
                }
            }

            EndWizard();
        }
    }

    void ModelSerializeWizard::SetInputFilePath(const std::filesystem::path& inputFilePath) noexcept
    {
        std::optional<LoadedModel> loadedModel = LoadModel(fileManager, inputFilePath);
        if (!loadedModel.has_value())
        {
            FileInputWizard::SetInputFilePath({ });
            model = std::nullopt;

            APP_WARNING("File [{0}] cannot be serialized, as its model data failed to load", inputFilePath.string());
            return;
        }

        FileInputWizard::SetInputFilePath(inputFilePath);
        FileOutputWizard::SetOutputFilePath(std::filesystem::path(inputFilePath).replace_extension(OUTPUT_FILE_EXTENSION));
        DefaultMetadata(metadata, inputFilePath.stem().string(), platformContext);

        model = std::move(loadedModel);
    }

    bool ModelSerializeWizard::Serialize() noexcept
    {
        if (!model.has_value())
        {
            return false;
        }

        const std::filesystem::path& outputDataFilePath = GetOutputFilePath();
        if (outputDataFilePath.empty())
        {
            APP_WARNING("Cannot serialize model [{0}] at invalid location [{1}]", metadata.name, outputDataFilePath.string());
            return false;
        }

        const ModelSerializeFormat format = static_cast<ModelSerializeFormat>(modelFormatIndex);
        std::unique_ptr<ModelSerializer> serializer = nullptr;

        switch (format)
        {
            case ModelSerializeFormat::YAML: { serializer = std::make_unique<YAMLModelSerializer>(); break; }
        }

        std::unique_ptr<MeshOptimizer> meshOptimizer = nullptr;
        switch (meshOptimization)
        {
            case MeshOptimization::None: { break; }
            case MeshOptimization::Zeux: { meshOptimizer = std::make_unique<ZeuxMeshOptimizer>(); break; }
        }

        const ModelSerializeInfo serializeInfo
        {
            .metadata = metadata,
            .settings = settings,
            .textureCompressionSettings = textureCompressionSettings,
            .meshCompressionSettings = meshCompressionSettings,
            .meshOptimizer = meshOptimizer.get(),
            .model = *model
        };

        ModelID ID = { };
        SerializedModelDependencies dependencies = { };
        const std::optional<SerializedModel> serializedModel = serializer->Serialize(serializeInfo, dependencies, ID);

        if (!serializedModel.has_value())
        {
            APP_WARNING("Could not serialize model [{0}]", metadata.name);
            return false;
        }

        WriteAssetFiles(fileManager, outputDataFilePath, ASSET_OUTPUT_FILE_BLOB_EXTENSION, serializedModel->data, serializedModel->blob);

        const std::filesystem::path outputDirectoryPath = outputDataFilePath.parent_path() / GetInputFilePath().stem();
        for (const std::optional<SerializedModelTextureDependency>& texture : dependencies.textures)
        {
            if (!texture.has_value()) continue;

            const std::filesystem::path filePath = outputDirectoryPath / "Textures" / SR_FORMAT("{0}{1}", texture->name, TextureSerializeWizard::OUTPUT_FILE_EXTENSION);
            WriteAssetFiles(fileManager, filePath, ASSET_OUTPUT_FILE_BLOB_EXTENSION, texture->texture.data, texture->texture.blob);
        }

        for (const std::optional<SerializedModelMaterialDependency>& material : dependencies.materials)
        {
            if (!material.has_value()) continue;

            const std::filesystem::path filePath = outputDirectoryPath / "Materials" / SR_FORMAT("{0}{1}", material->name, MaterialSerializeWizard::OUTPUT_FILE_EXTENSION);
            WriteAssetFiles(fileManager, filePath, ASSET_OUTPUT_FILE_BLOB_EXTENSION, material->material.data, material->material.blob);
        }

        return true;
    }

    void ModelSerializeWizard::DrawSerializeInfoMenu() noexcept
    {
        if (ImGui::TreeNodeEx("Serialize Info", MENU_TREE_FLAGS))
        {
            if (ImGuiWidgets::BeginPropertyTable())
            {
                ImGuiWidgets::BeginProperty("Mesh Optimization");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "None" },
                        ImGuiDropdownOption { .text = "Zeux" }
                    };

                    if (uint32 meshOptimizationIndex = static_cast<uint32>(meshOptimization); ImGuiWidgets::Dropdown("##MeshOptimizationDropdown", meshOptimizationIndex, { .options = OPTIONS }))
                    {
                        meshOptimization = static_cast<MeshOptimization>(meshOptimizationIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Mesh Compression");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "None" }
                    };

                    if (uint32 meshCompressionIndex = static_cast<uint32>(meshCompressionSettings.compression); ImGuiWidgets::Dropdown("##MeshCompressionDropdown", meshCompressionIndex, { .options = OPTIONS }))
                    {
                        meshCompressionSettings.compression = static_cast<MeshCompression>(meshCompressionIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Texture Compression");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "None" },
                        ImGuiDropdownOption { .text = "Basis Universal" }
                    };

                    if (uint32 textureCompressionIndex = static_cast<uint32>(textureCompressionSettings.compression); ImGuiWidgets::Dropdown("##TextureCompressionDropdown", textureCompressionIndex, { .options = OPTIONS }))
                    {
                        textureCompressionSettings.compression = static_cast<TextureCompression>(textureCompressionIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                if (textureCompressionSettings.compression != TextureCompression::None)
                {
                    constexpr std::array LEVEL_OPTIONS
                    {
                        ImGuiDropdownOption { .text = "Lowest" },
                        ImGuiDropdownOption { .text = "Low" },
                        ImGuiDropdownOption { .text = "Standard" },
                        ImGuiDropdownOption { .text = "High" },
                        ImGuiDropdownOption { .text = "Highest" }
                    };

                    ImGuiWidgets::BeginProperty("Texture Compression Level");
                    {
                        if (uint32 compressionLevelIndex = static_cast<uint32>(textureCompressionSettings.aggressiveness); ImGuiWidgets::Dropdown("##TextureCompressionAggressivenessDropdown", compressionLevelIndex, { .options = LEVEL_OPTIONS }))
                        {
                            textureCompressionSettings.aggressiveness = static_cast<TextureCompressionAggressiveness>(compressionLevelIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::BeginProperty("Texture Quality Level");
                    {
                        if (uint32 qualityLevelIndex = static_cast<uint32>(textureCompressionSettings.quality); ImGuiWidgets::Dropdown("##TextureQualityDropdown", qualityLevelIndex, { .options = LEVEL_OPTIONS }))
                        {
                            textureCompressionSettings.quality = static_cast<TextureCompressionQuality>(qualityLevelIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();
                }

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

}
