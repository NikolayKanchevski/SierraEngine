//
// Created by Nikolay Kanchevski on 7.11.24.
//

#include "TextureSerializeWizard.h"

#include "../../../Assets/Textures/Loaders/AutoImageLoader.h"
#include "../../../Assets/Textures/Serializers/YAMLTextureSerializer.h"

namespace SierraEngine
{

    namespace
    {
        std::optional<LoadedImage> LoadLayerImage(const Sierra::FileManager& fileManager, const std::filesystem::path& filePath) noexcept
        {
            const std::vector<uint8> memory = fileManager.ReadFile(filePath);
            if (memory.empty())
            {
                APP_WARNING("Failed to read image file [{0}]", filePath.string());
                return std::nullopt;
            }

            std::optional<LoadedImage> image = AutoImageLoader::Load({
                .memory = memory,
                .loadChannels = ImageLoadChannels::All
            });

            if (!image.has_value())
            {
                APP_WARNING("Failed to load image [{0}]", filePath.string());
                return std::nullopt;
            }

            return image;
        }
    }

    /* --- CONSTRUCTORS --- */

    TextureSerializeWizard::TextureSerializeWizard(const TextureSerializeWizardCreateInfo& createInfo)
        : FileInputWizard({
            .platformContext = createInfo.platformContext,
            .inputFileExtensions = INPUT_FILE_EXTENSIONS,
            .inputFilePath = createInfo.inputFilePath,
            .outputFileExtension = OUTPUT_FILE_EXTENSION,
            .outputDirectoryPath = createInfo.outputDirectoryPath
          }),
          AssetSerializeWizard({
            .platformContext = createInfo.platformContext,
            .inputFileExtensions = INPUT_FILE_EXTENSIONS,
            .inputFilePath = createInfo.inputFilePath,
            .outputFileExtension = OUTPUT_FILE_EXTENSION,
            .outputDirectoryPath = createInfo.outputDirectoryPath
          }),
          renderingContext(&createInfo.renderingContext)
    {
        if (createInfo.inputFilePath != nullptr)
        {
            SetInputFilePath(*createInfo.inputFilePath);
        }
    }

    /* --- POLLING METHODS --- */

    void TextureSerializeWizard::Draw(bool& open, Sierra::CommandBuffer& commandBuffer)
    {
        if (ImGui::IsKeyPressed(ImGuiKey_Escape))
        {
            open = false;

            ResetLayers(commandBuffer);
            return;
        }

        if (rootLayerDirty)
        {
            ReloadRootLayer(commandBuffer);
            rootLayerDirty = false;
        }

        if (BeginWizard("Texture Serialize Wizard", open))
        {
            DrawFileInputHeader();
            DrawMetadataMenu(serializeInfo.metadata);

            DrawPropertiesMenu();
            DrawSerializeInfoMenu();
            DrawExtrasMenu(commandBuffer);

            constexpr std::array OPTIONS
            {
                ImGuiDropdownOption { .text = "YAML" }
            };

            if (DrawFileOutputFooter(textureFormatIndex, OPTIONS))
            {
                if (Serialize())
                {
                    open = false;
                }
            }

            EndWizard();
        }

        if (!open) ResetLayers(commandBuffer);
    }

    void TextureSerializeWizard::SetInputFilePath(const std::filesystem::path& inputFilePath) noexcept
    {
        std::optional<LoadedImage> image = LoadLayerImage(FileInputWizard::GetPlatformContext().GetFileManager(), inputFilePath);
        if (!image.has_value())
        {
            FileInputWizard::SetInputFilePath({ });

            APP_WARNING("File [{0}] cannot be serialized, as its image data failed to load", inputFilePath.string());
            return;
        }

        FileInputWizard::SetInputFilePath(inputFilePath);
        FileOutputWizard::SetOutputFilePath(std::filesystem::path(inputFilePath).replace_extension(OUTPUT_FILE_EXTENSION));
        DefaultMetadata(serializeInfo.metadata, inputFilePath.stem().string());

        if (levels.empty() || levels[0].layers.empty())
        {
            levels.resize(1);
            levels[0].layers.resize(1);
        }

        ImageLayerData& rootLayer = levels[0].layers[0];
        rootLayer.dirty = true;
        rootLayer.filePath = inputFilePath;
        rootLayer.image = std::move(image);

        rootLayerDirty = true;
    }

    TextureSerializeWizard::ImageLayerData* TextureSerializeWizard::GetRootLayer() noexcept
    {
        return (!levels.empty() && !levels[0].layers.empty()) ? &levels[0].layers[0] : nullptr;
    }

    LoadedImage* TextureSerializeWizard::GetRootImage() noexcept
    {
        if (ImageLayerData* rootLayer = GetRootLayer(); rootLayer->image.has_value())
        {
            return &(*rootLayer->image);
        }
        return nullptr;
    }

    bool TextureSerializeWizard::ImageMatchesLevelDimensions(const LoadedImage& image, const uint32 levelIndex)
    {
        const LoadedImage* rootImage = GetRootImage();
        if (rootImage == nullptr) return true;

        return image.width == glm::max(1U, rootImage->width >> levelIndex) && image.height == glm::max(1U, rootImage->height >> levelIndex);
    }

    void TextureSerializeWizard::ReloadRootLayer(Sierra::CommandBuffer& commandBuffer) noexcept
    {
        ImageLayerData* rootLayer = GetRootLayer();
        if (rootLayer == nullptr) return;

        const std::filesystem::path& inputFilePath = rootLayer->filePath;
        std::optional<LoadedImage> image = LoadLayerImage(FileInputWizard::GetPlatformContext().GetFileManager(), inputFilePath);
        if (!image.has_value())
        {
            FileInputWizard::SetInputFilePath({ });

            APP_WARNING("File [{0}] cannot be serialized, as its image data failed to load", inputFilePath.string());
            return;
        }

        FileInputWizard::SetInputFilePath(inputFilePath);
        FileOutputWizard::SetOutputFilePath(std::filesystem::path(inputFilePath).replace_extension(OUTPUT_FILE_EXTENSION));
        DefaultMetadata(serializeInfo.metadata, inputFilePath.stem().string());

        for (uint32 level = 0; level < levels.size(); level++)
        {
            for (uint32 layer = 0; layer < levels[level].layers.size(); layer++)
            {
                if (level == 0 && layer == 0) continue;

                ImageLayerData& layerData = levels[level].layers[layer];
                ResetLayer(layerData, commandBuffer);
            }
        }

        ResetLayer(*rootLayer, commandBuffer);

        std::optional<ImagePreview> preview = ImagePreviewer::Preview({ .renderingContext = *renderingContext, .commandBuffer = commandBuffer, .image = *image });
        if (preview.has_value())
        {
            Sierra::ResourceTable& resourceTable = renderingContext->GetResourceTable();
            preview->ID = resourceTable.BindSampledImage(*preview->image);
        }

        *rootLayer =
        {
            .dirty = false,
            .filePath = inputFilePath,
            .image = std::move(image),
            .preview = std::move(preview)
        };
    }

    void TextureSerializeWizard::ReloadLevels(Sierra::CommandBuffer& commandBuffer) noexcept
    {
        const LoadedImage* rootImage = GetRootImage();
        if (rootImage == nullptr) return;

        const uint32 levelCount = ((levelOptions == LevelOptions::Load) * glm::log2(glm::max(rootImage->width, rootImage->height))) + 1;
        if (levels.size() == levelCount) return;

        for (uint32 level = 1; level < levels.size(); level++)
        {
            for (uint32 layer = 0; layer < levels[level].layers.size(); layer++)
            {
                if (level == 0 && layer == 0) continue;

                ImageLayerData& layerData = levels[level].layers[layer];
                ResetLayer(layerData, commandBuffer);
            }
        }

        levels.resize(1);
        levels.resize(levelCount);

        for (ImageLevelData& level : levels)
        {
            level.layers.resize(layerCount);
        }
    }

    void TextureSerializeWizard::ReloadLayers(uint32 lastLayerCount, Sierra::CommandBuffer& commandBuffer) noexcept
    {
        if (layerCount < lastLayerCount)
        {
            for (uint32 level = 0; level < levels.size(); level++)
            {
                for (uint32 layer = lastLayerCount - 1; layer < lastLayerCount; layer++)
                {
                    if (level == 0 && layer == 0) continue;

                    ImageLayerData& layerData = levels[level].layers[layer];
                    ResetLayer(layerData, commandBuffer);
                }
            }
        }

        for (ImageLevelData& level : levels)
        {
            level.layers.resize(layerCount);
        }
    }

    void TextureSerializeWizard::ResetLayers(Sierra::CommandBuffer& commandBuffer) noexcept
    {
        for (ImageLevelData& level : levels)
        {
            for (ImageLayerData& layer : level.layers)
            {
                ResetLayer(layer, commandBuffer);
            }
        }
    }

    void TextureSerializeWizard::ResetLayer(ImageLayerData& layer, Sierra::CommandBuffer& commandBuffer) const noexcept
    {
        if (!layer.preview.has_value()) return;

        Sierra::DestructionScheduler& destructionScheduler = renderingContext->GetDestructionScheduler();
        destructionScheduler.QueueResource(std::move(layer.preview->image));

        Sierra::ResourceTable& resourceTable = renderingContext->GetResourceTable();
        resourceTable.FreeSampledImage(layer.preview->ID);

        layer.preview = std::nullopt;
    }

    bool TextureSerializeWizard::Serialize() noexcept
    {
        if (levels.empty() || levels[0].layers.empty() || !levels[0].layers[0].image.has_value())
        {
            return false;
        }

        std::vector<LoadedImageLevel> serializeLevels(levels.size());
        std::vector<std::vector<LoadedImage>> serializeLayers(levels.size());

        for (uint32 level = 0; level < levels.size(); level++)
        {
            serializeLayers[level].resize(layerCount);
            for (uint32 layer = 0; layer < levels[level].layers.size(); layer++)
            {
                std::optional<LoadedImage>& layerImage = levels[level].layers[layer].image;
                if (!layerImage.has_value())
                {
                    APP_WARNING("Cannot serialize texture [{0}] if level options are set to [Load], unless all layers have an image file specified");
                    return false;
                }

                serializeLayers[level][layer] = std::move(layerImage.value());
            }

            serializeLevels[level].layers = serializeLayers[level];
        }
        serializeInfo.levels = serializeLevels;

        const TextureSerializeFormat format = static_cast<TextureSerializeFormat>(textureFormatIndex);
        std::unique_ptr<TextureSerializer> serializer = nullptr;

        switch (format)
        {
            case TextureSerializeFormat::YAML: { serializer = std::make_unique<YAMLTextureSerializer>(); break; }
        }

        const std::optional<SerializedTexture> serializedTexture = serializer->Serialize(serializeInfo);
        if (!serializedTexture.has_value())
        {
            APP_WARNING("Could not serialize texture [{0}]", serializeInfo.metadata.name);
            return false;
        }

        const Sierra::FileManager& fileManager = FileOutputWizard::GetPlatformContext().GetFileManager();

        const std::filesystem::path& outputDataFilePath = GetOutputFilePath();
        fileManager.CreateFile(outputDataFilePath, Sierra::FilePathConflictPolicy::Overwrite);
        fileManager.WriteFile(outputDataFilePath, serializedTexture->data);

        const std::filesystem::path outputBlobFilePath = std::filesystem::path(outputDataFilePath).replace_extension(ASSET_OUTPUT_FILE_BLOB_EXTENSION);
        fileManager.CreateFile(outputBlobFilePath, Sierra::FilePathConflictPolicy::Overwrite);
        fileManager.WriteFile(outputBlobFilePath, serializedTexture->blob);

        return true;
    }

    void TextureSerializeWizard::DrawPropertiesMenu() noexcept
    {
        if (ImGui::TreeNodeEx("Properties", MENU_TREE_FLAGS))
        {
            if (ImGuiWidgets::BeginPropertyTable())
            {
                ImGuiWidgets::BeginProperty("Filter");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "Pixelated" },
                        ImGuiDropdownOption { .text = "Smooth" }
                    };

                    if (uint32 filterTypeIndex = static_cast<uint32>(serializeInfo.properties.filter); ImGuiWidgets::Dropdown("##FilterDropdown", filterTypeIndex, { .options = OPTIONS }))
                    {
                        serializeInfo.properties.filter = static_cast<TextureFilter>(filterTypeIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

    void TextureSerializeWizard::DrawSerializeInfoMenu() noexcept
    {
        if (ImGui::TreeNodeEx("Serialize Info", MENU_TREE_FLAGS))
        {
            if (ImGuiWidgets::BeginPropertyTable())
            {
                ImGuiWidgets::BeginProperty("Compression");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "None" },
                        ImGuiDropdownOption { .text = "Basis Universal" }
                    };

                    if (uint32 compressorTypeIndex = static_cast<uint32>(serializeInfo.compression); ImGuiWidgets::Dropdown("##CompressorTypeDropdown", compressorTypeIndex, { .options = OPTIONS }))
                    {
                        serializeInfo.compression = static_cast<ImageCompression>(compressorTypeIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                if (serializeInfo.compression != ImageCompression::None)
                {
                    ImGuiWidgets::BeginProperty("Compression Level");
                    {
                        constexpr std::array OPTIONS
                        {
                            ImGuiDropdownOption { .text = "Lowest" },
                            ImGuiDropdownOption { .text = "Low" },
                            ImGuiDropdownOption { .text = "Standard" },
                            ImGuiDropdownOption { .text = "High" },
                            ImGuiDropdownOption { .text = "Highest" }
                        };

                        if (uint32 compressionLevelIndex = static_cast<uint32>(serializeInfo.compressionLevel); ImGuiWidgets::Dropdown("##CompressionLevelDropdown", compressionLevelIndex, { .options = OPTIONS }))
                        {
                            serializeInfo.compressionLevel = static_cast<ImageCompressionLevel>(compressionLevelIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::BeginProperty("Quality Level");
                    {
                        constexpr std::array OPTIONS
                        {
                            ImGuiDropdownOption { .text = "Lowest" },
                            ImGuiDropdownOption { .text = "Low" },
                            ImGuiDropdownOption { .text = "Standard" },
                            ImGuiDropdownOption { .text = "High" },
                            ImGuiDropdownOption { .text = "Highest" }
                        };

                        if (uint32 qualityLevelIndex = static_cast<uint32>(serializeInfo.compressionQualityLevel); ImGuiWidgets::Dropdown("##QualityLevelDropdown", qualityLevelIndex, { .options = OPTIONS }))
                        {
                            serializeInfo.compressionQualityLevel = static_cast<ImageCompressionQualityLevel>(qualityLevelIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();
                }

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

    void TextureSerializeWizard::DrawExtrasMenu(Sierra::CommandBuffer& commandBuffer) noexcept
    {
        if (ImGui::TreeNodeEx("Extras", MENU_TREE_FLAGS))
        {
            if (ImGuiWidgets::BeginPropertyTable({ .tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInner }))
            {
                ImGuiWidgets::BeginProperty("Level Options");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "None" },
                        ImGuiDropdownOption { .text = "Load" },
                    };

                    if (uint32 levelOptionsIndex = static_cast<uint32>(levelOptions); ImGuiWidgets::Dropdown("##LevelOptionsDropdown", levelOptionsIndex, { .options = OPTIONS }))
                    {
                        levelOptions = static_cast<LevelOptions>(levelOptionsIndex);
                        ReloadLevels(commandBuffer);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Layer Count");
                {
                    uint32 lastLayerCount = layerCount;
                    if (ImGuiWidgets::NumericEnterInput("##LayerCountInput", layerCount, 1U, 16U))
                    {
                        ReloadLayers(lastLayerCount, commandBuffer);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Levels");
                {
                    const ImGuiListInputItemCallback<ImageLevelData> ItemCallback = [this, &commandBuffer](ImageLevelData& level, const uint32 levelIndex) -> void
                    {
                        const ImGuiStyle& style = ImGui::GetStyle();
                        const float32 availableHorizontalSpace = ImGui::GetContentRegionAvail().x;

                        const uint32 levelsPerRow = glm::min(size(2), level.layers.size());
                        const float32 size = (availableHorizontalSpace - (2.0f * static_cast<float32>(levelsPerRow - 1) + static_cast<float32>(levelsPerRow > 1) * 2.0f) * style.CellPadding.x) / static_cast<float32>(levelsPerRow) * (layerCount == 1 ? 0.35f : 1.0f);

                        if (level.layers.size() == 1)
                        {
                            ImGuiWidgets::BeginHorizontalPadding((availableHorizontalSpace - size) / 2.0f);
                        }

                        if (ImGui::BeginTable("##LevelsTable", static_cast<int>(levelsPerRow)))
                        {
                            ImGui::TableNextRow();

                            for (uint32 layerIndex = 0; layerIndex < level.layers.size(); layerIndex++)
                            {
                                ImGui::TableNextColumn();
                                ImGui::PushID(static_cast<int>(layerIndex));

                                ImageLayerData& layer = level.layers[layerIndex];
                                if (layer.dirty)
                                {
                                    std::optional<ImagePreview> preview = ImagePreviewer::Preview({ .renderingContext = *renderingContext, .commandBuffer = commandBuffer, .image = *layer.image });
                                    ResetLayer(layer, commandBuffer);

                                    Sierra::ResourceTable& resourceTable = renderingContext->GetResourceTable();
                                    layer.dirty = false;

                                    layer.preview = std::move(preview.value());
                                    layer.preview->ID = resourceTable.BindSampledImage(*layer.preview->image);
                                }

                                const ImGuiImageInputInfo inputInfo
                                {
                                    .previewID = layer.preview.has_value() ? std::optional(layer.preview->ID) : std::nullopt,
                                    .allowedFileExtensions = INPUT_FILE_EXTENSIONS,
                                    .imageWidth = layer.image.has_value() ? layer.image->width : 0,
                                    .imageHeight = layer.image.has_value() ? layer.image->height : 0,
                                    .width = size
                                };

                                if (ImGuiWidgets::ImageInput("##LevelsLayerImageInput", layer.filePath, FileInputWizard::GetPlatformContext(), inputInfo))
                                {
                                    const Sierra::FileManager& fileManager = FileInputWizard::GetPlatformContext().GetFileManager();
                                    if (std::optional<LoadedImage> layerImage = LoadLayerImage(fileManager, layer.filePath); layerImage.has_value())
                                    {
                                        const bool rootLayer = levelIndex == 0 && layerIndex == 0;
                                        if (!rootLayer && !ImageMatchesLevelDimensions(*layerImage, levelIndex))
                                        {
                                            APP_WARNING("Cannot use image [{0}] as level [{1}] layer [{2}] of texture [{3}], as its dimensions do not match those of the level", layer.filePath.string(), levelIndex, layerIndex, serializeInfo.metadata.name);
                                        }
                                        else
                                        {
                                            layer.image.emplace(std::move(layerImage.value()));
                                            layer.dirty = true;
                                        }

                                        rootLayerDirty = rootLayer;
                                    }
                                }

                                if ((layerIndex + 1) % levelsPerRow == 0)
                                {
                                    ImGui::TableNextRow();
                                }

                                ImGui::PopID();
                            }
                            ImGui::EndTable();
                        }

                        if (level.layers.size() == 1)
                        {
                            ImGuiWidgets::EndPadding();
                        }
                    };

                    const ImGuiListInputLabelCallback LabelCallback = [this](const uint32 i) -> std::string
                    {
                        if (const LoadedImage* rootImage = GetRootImage(); rootImage != nullptr)
                        {
                            return SR_FORMAT("{0}x{1}:", glm::max(1U, rootImage->width >> i), glm::max(1U, rootImage->height >> i));
                        }

                        return SR_FORMAT("{0}", i);
                    };

                    ImGuiWidgets::ListInput("##LevelsListInput", levels, { .allowOperations = false, .ItemCallback = ItemCallback, .LabelCallback = LabelCallback });
                }
                ImGuiWidgets::EndProperty();
            }
            ImGuiWidgets::EndPropertyTable();
            ImGui::TreePop();
        }
    }

}