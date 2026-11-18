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
        constexpr float32 SINGLE_LAYER_PREVIEW_SCALE = 0.35f;

        constexpr std::array COMPRESSION_LEVEL_OPTIONS
        {
            ImGuiDropdownOption { .text = "Lowest" },
            ImGuiDropdownOption { .text = "Low" },
            ImGuiDropdownOption { .text = "Standard" },
            ImGuiDropdownOption { .text = "High" },
            ImGuiDropdownOption { .text = "Highest" }
        };

        std::optional<LoadedImage> LoadImage(const Sierra::FileManager& fileManager, const Sierra::Path& filePath) noexcept
        {
            std::vector<uint8> memory = { };
            static_cast<void>(fileManager.ReadFile(filePath, memory));

            if (memory.empty())
            {
                APP_WARNING("Failed to read image file [{0}]", filePath.GetString());
                return std::nullopt;
            }

            std::optional<LoadedImage> image = AutoImageLoader::Load({
                .buffer = memory,
                .loadChannels = ImageLoadChannels::All
            });

            if (!image.has_value())
            {
                APP_WARNING("Failed to load image [{0}]", filePath.GetString());
                return std::nullopt;
            }

            return image;
        }

        bool HasImage(const LoadedImage& image) noexcept
        {
            return !image.buffer.empty();
        }

        void FreePreview(const DrawFrame& drawFrame, std::optional<ImagePreview>& preview) noexcept
        {
            if (!preview.has_value())
                return;

            drawFrame.destructionScheduler.QueueResource(std::move(preview->image));
            drawFrame.resourceTable.FreeSampledImage(preview->ID);

            preview = std::nullopt;
        }
    }

    /* --- CONSTRUCTORS --- */

    TextureSerializeWizard::TextureSerializeWizard(const TextureSerializeWizardCreateInfo& createInfo)
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
          })
    {
        DefaultMetadata(serializeInfo.metadata, "Texture", createInfo.platformContext);

        // The root level and layer always exist, so the source image always has a slot
        levels.resize(1);
        levels.front().images.resize(layerCount);
        levels.front().layers.resize(layerCount);

        if (createInfo.inputFilePath != nullptr)
        {
            pendingInputFilePath = *createInfo.inputFilePath;
        }
    }

    /* --- POLLING METHODS --- */

    bool TextureSerializeWizard::Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame)
    {
        if (const std::optional<Sierra::Path> inputFilePath = std::exchange(pendingInputFilePath, std::nullopt); inputFilePath.has_value())
        {
            LoadInputFile(applicationFrame, drawFrame, *inputFilePath);
        }
        UpdatePreviews(drawFrame);

        bool open = BeginWizard("Texture Serialize Wizard");
        if (open)
        {
            DrawFileInputHeader(applicationFrame.platformContext);
            DrawMetadataMenu(serializeInfo.metadata);

            DrawSettingsMenu();
            DrawSerializeInfoMenu();
            DrawExtrasMenu(applicationFrame, drawFrame);

            constexpr std::array FORMAT_OPTIONS
            {
                ImGuiDropdownOption { .text = "YAML" }
            };

            if (DrawFileOutputFooter(applicationFrame.platformContext, textureFormatIndex, FORMAT_OPTIONS) && Serialize(applicationFrame))
            {
                open = false;
            }

            EndWizard();
        }

        if (!open)
        {
            FreePreviews(drawFrame);
        }

        return open;
    }

    /* --- SETTER METHODS --- */

    void TextureSerializeWizard::SetInputFilePath(const Sierra::Path& inputFilePath) noexcept
    {
        // Loading needs the current frame (for the file manager and to create previews), so it is deferred to the start of the next draw
        pendingInputFilePath = inputFilePath;
    }

    /* --- PRIVATE METHODS --- */

    void TextureSerializeWizard::LoadInputFile(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame, const Sierra::Path& inputFilePath) noexcept
    {
        LoadedImage& rootImage = levels.front().images.front();
        ImageLayerData& rootLayer = levels.front().layers.front();

        std::optional<LoadedImage> image = LoadImage(applicationFrame.platformContext.GetFileManager(), inputFilePath);
        if (!image.has_value())
        {
            // Keep showing the source that is actually loaded
            FileInputWizard::SetInputFilePath(rootLayer.filePath);
            return;
        }

        const bool layoutChanged = image->width != rootImage.width || image->height != rootImage.height || image->format != rootImage.format;

        rootImage = std::move(*image);
        rootLayer.filePath = inputFilePath;
        rootLayer.previewOutdated = true;

        if (layoutChanged)
        {
            // Every other level and layer must match the source's dimensions and format, so the level count is recalculated and layers that no longer fit are dropped
            ResizeLevels(drawFrame);
            for (uint32 levelIndex = 0; levelIndex < levels.size(); levelIndex++)
            {
                ImageLevelData& level = levels[levelIndex];
                for (uint32 layerIndex = (levelIndex == 0 ? 1 : 0); layerIndex < level.layers.size(); layerIndex++)
                {
                    LoadedImage& layerImage = level.images[layerIndex];
                    if (!HasImage(layerImage) || FitsLevel(layerImage, levelIndex))
                        continue;

                    ImageLayerData& layer = level.layers[layerIndex];
                    FreePreview(drawFrame, layer.preview);

                    layerImage = { };
                    layer = { };
                }
            }
        }

        FileInputWizard::SetInputFilePath(inputFilePath);
        serializeInfo.metadata.name = inputFilePath.GetStem();

        // The output stays in its directory (which may have been picked by the user or the wizard's creator), but follows the source's name
        const Sierra::Path& outputFilePath = GetOutputFilePath();
        const Sierra::Path outputDirectoryPath = !outputFilePath.IsEmpty() ? outputFilePath.GetParentPath() : inputFilePath.GetParentPath();
        FileOutputWizard::SetOutputFilePath(outputDirectoryPath / Sierra::Path(inputFilePath.GetFileName()).ChangeExtension(OUTPUT_FILE_EXTENSION));
    }

    void TextureSerializeWizard::LoadLayer(const ApplicationFrame& applicationFrame, ImageLevelData& level, const uint32 levelIndex, const uint32 layerIndex, const Sierra::Path& filePath) noexcept
    {
        std::optional<LoadedImage> image = LoadImage(applicationFrame.platformContext.GetFileManager(), filePath);
        if (!image.has_value())
            return;

        if (!FitsLevel(*image, levelIndex))
        {
            const Vector2UInt levelDimensions = GetLevelDimensions(levelIndex);
            APP_WARNING("Cannot use image [{0}] as level [{1}] layer [{2}] of texture [{3}], as it must be {4}x{5} and share the format of the source image", filePath.GetString(), levelIndex, layerIndex, serializeInfo.metadata.name, levelDimensions.x, levelDimensions.y);
            return;
        }

        level.images[layerIndex] = std::move(*image);

        ImageLayerData& layer = level.layers[layerIndex];
        layer.filePath = filePath;
        layer.previewOutdated = true;
    }

    uint32 TextureSerializeWizard::GetLevelCount() const noexcept
    {
        const LoadedImage& rootImage = GetRootImage();
        if (levelOptions == LevelOptions::None || !HasImage(rootImage))
            return 1;

        // Full chain down to 1x1, calculated the same way the serializer validates it
        return glm::log2(glm::max(rootImage.width, rootImage.height)) + 1;
    }

    Vector2UInt TextureSerializeWizard::GetLevelDimensions(const uint32 levelIndex) const noexcept
    {
        const LoadedImage& rootImage = GetRootImage();
        return { glm::max(1U, rootImage.width >> levelIndex), glm::max(1U, rootImage.height >> levelIndex) };
    }

    bool TextureSerializeWizard::FitsLevel(const LoadedImage& image, const uint32 levelIndex) const noexcept
    {
        // Without a source image there is nothing to compare against yet; once one is loaded, layers that do not fit are dropped
        const LoadedImage& rootImage = GetRootImage();
        if (!HasImage(rootImage))
            return true;

        return Vector2UInt(image.width, image.height) == GetLevelDimensions(levelIndex) && image.format == rootImage.format;
    }

    void TextureSerializeWizard::ResizeLevels(const DrawFrame& drawFrame) noexcept
    {
        const uint32 levelCount = GetLevelCount();
        for (uint32 levelIndex = levelCount; levelIndex < levels.size(); levelIndex++)
        {
            for (ImageLayerData& layer : levels[levelIndex].layers)
            {
                FreePreview(drawFrame, layer.preview);
            }
        }
        levels.resize(levelCount);

        for (ImageLevelData& level : levels)
        {
            for (uint32 layerIndex = layerCount; layerIndex < level.layers.size(); layerIndex++)
            {
                FreePreview(drawFrame, level.layers[layerIndex].preview);
            }

            level.images.resize(layerCount);
            level.layers.resize(layerCount);
        }
    }

    void TextureSerializeWizard::UpdatePreviews(const DrawFrame& drawFrame) noexcept
    {
        for (ImageLevelData& level : levels)
        {
            for (uint32 layerIndex = 0; layerIndex < level.layers.size(); layerIndex++)
            {
                ImageLayerData& layer = level.layers[layerIndex];
                if (!layer.previewOutdated)
                    continue;

                layer.previewOutdated = false;

                std::optional<ImagePreview> preview = ImagePreviewer::Preview(drawFrame, { .image = level.images[layerIndex] });
                if (preview.has_value())
                {
                    preview->ID = drawFrame.resourceTable.BindSampledImage(*preview->image);
                }
                else
                {
                    APP_WARNING("Could not create a preview of image [{0}]", layer.filePath.GetString());
                }

                FreePreview(drawFrame, layer.preview);
                layer.preview = std::move(preview);
            }
        }
    }

    void TextureSerializeWizard::FreePreviews(const DrawFrame& drawFrame) noexcept
    {
        for (ImageLevelData& level : levels)
        {
            for (ImageLayerData& layer : level.layers)
            {
                FreePreview(drawFrame, layer.preview);
            }
        }
    }

    bool TextureSerializeWizard::Serialize(const ApplicationFrame& applicationFrame) const noexcept
    {
        const Sierra::Path& outputFilePath = GetOutputFilePath();
        if (outputFilePath.IsEmpty())
        {
            APP_WARNING("Cannot serialize texture [{0}], as no output location has been specified", serializeInfo.metadata.name);
            return false;
        }

        std::vector<LoadedImageLevel> serializeLevels = { };
        serializeLevels.reserve(levels.size());

        for (uint32 levelIndex = 0; levelIndex < levels.size(); levelIndex++)
        {
            const ImageLevelData& level = levels[levelIndex];
            for (uint32 layerIndex = 0; layerIndex < level.images.size(); layerIndex++)
            {
                if (!HasImage(level.images[layerIndex]))
                {
                    APP_WARNING("Cannot serialize texture [{0}], as level [{1}] layer [{2}] has no image assigned", serializeInfo.metadata.name, levelIndex, layerIndex);
                    return false;
                }
            }

            serializeLevels.push_back({ .layers = level.images });
        }

        TextureSerializeInfo textureSerializeInfo = serializeInfo;
        textureSerializeInfo.levels = serializeLevels;

        std::unique_ptr<TextureSerializer> serializer = nullptr;
        switch (static_cast<TextureSerializeFormat>(textureFormatIndex))
        {
            case TextureSerializeFormat::YAML: { serializer = std::make_unique<YAMLTextureSerializer>(); break; }
        }

        TextureID ID = { };
        const std::optional<SerializedTexture> serializedTexture = serializer->Serialize(textureSerializeInfo, ID);
        if (!serializedTexture.has_value())
        {
            APP_WARNING("Could not serialize texture [{0}]", serializeInfo.metadata.name);
            return false;
        }

        if (!WriteAssetFiles(applicationFrame.platformContext.GetFileManager(), outputFilePath, *serializedTexture))
        {
            APP_WARNING("Failed to write serialized texture [{0}] to [{1}]", serializeInfo.metadata.name, outputFilePath.GetString());
            return false;
        }

        return true;
    }

    void TextureSerializeWizard::DrawSettingsMenu() noexcept
    {
        if (ImGui::TreeNodeEx("Settings", MENU_TREE_FLAGS))
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

                    if (uint32 filterIndex = static_cast<uint32>(serializeInfo.settings.filter); ImGuiWidgets::Dropdown("##FilterDropdown", filterIndex, { .options = OPTIONS }))
                    {
                        serializeInfo.settings.filter = static_cast<TextureFilter>(filterIndex);
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
                TextureCompressionSettings& compressionSettings = serializeInfo.compressionSettings;

                ImGuiWidgets::BeginProperty("Compression");
                {
                    constexpr std::array OPTIONS
                    {
                        ImGuiDropdownOption { .text = "None" },
                        ImGuiDropdownOption { .text = "Basis Universal" }
                    };

                    if (uint32 compressionIndex = static_cast<uint32>(compressionSettings.compression); ImGuiWidgets::Dropdown("##CompressionDropdown", compressionIndex, { .options = OPTIONS }))
                    {
                        compressionSettings.compression = static_cast<TextureCompression>(compressionIndex);
                    }
                }
                ImGuiWidgets::EndProperty();

                if (compressionSettings.compression != TextureCompression::None)
                {
                    ImGuiWidgets::BeginProperty("Compression Level");
                    {
                        if (uint32 aggressivenessIndex = static_cast<uint32>(compressionSettings.aggressiveness); ImGuiWidgets::Dropdown("##CompressionAggressivenessDropdown", aggressivenessIndex, { .options = COMPRESSION_LEVEL_OPTIONS }))
                        {
                            compressionSettings.aggressiveness = static_cast<TextureCompressionAggressiveness>(aggressivenessIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();

                    ImGuiWidgets::BeginProperty("Quality Level");
                    {
                        if (uint32 qualityIndex = static_cast<uint32>(compressionSettings.quality); ImGuiWidgets::Dropdown("##QualityDropdown", qualityIndex, { .options = COMPRESSION_LEVEL_OPTIONS }))
                        {
                            compressionSettings.quality = static_cast<TextureCompressionQuality>(qualityIndex);
                        }
                    }
                    ImGuiWidgets::EndProperty();
                }

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

    void TextureSerializeWizard::DrawExtrasMenu(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame) noexcept
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
                        ImGuiDropdownOption { .text = "Load" }
                    };

                    if (uint32 levelOptionsIndex = static_cast<uint32>(levelOptions); ImGuiWidgets::Dropdown("##LevelOptionsDropdown", levelOptionsIndex, { .options = OPTIONS }))
                    {
                        levelOptions = static_cast<LevelOptions>(levelOptionsIndex);
                        ResizeLevels(drawFrame);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Layer Count");
                {
                    if (ImGuiWidgets::NumericEnterInput("##LayerCountInput", layerCount, 1U, MAX_LAYER_COUNT))
                    {
                        ResizeLevels(drawFrame);
                    }
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::BeginProperty("Levels");
                {
                    const ImGuiListInputItemCallback<ImageLevelData> ItemCallback = [this, &applicationFrame](ImageLevelData& level, const uint32 levelIndex) -> void
                    {
                        DrawLevel(applicationFrame, level, levelIndex);
                    };

                    const ImGuiListInputLabelCallback LabelCallback = [this](const uint32 levelIndex) -> std::string
                    {
                        if (!HasImage(GetRootImage()))
                            return SR_FORMAT("{0}", levelIndex);

                        const Vector2UInt levelDimensions = GetLevelDimensions(levelIndex);
                        return SR_FORMAT("{0}x{1}:", levelDimensions.x, levelDimensions.y);
                    };

                    ImGuiWidgets::ListInput("##LevelsListInput", levels, { .allowOperations = false, .ItemCallback = ItemCallback, .LabelCallback = LabelCallback });
                }
                ImGuiWidgets::EndProperty();

                ImGuiWidgets::EndPropertyTable();
            }
            ImGui::TreePop();
        }
    }

    void TextureSerializeWizard::DrawLevel(const ApplicationFrame& applicationFrame, ImageLevelData& level, const uint32 levelIndex) noexcept
    {
        const uint32 columnCount = glm::min(2U, layerCount);
        const float32 availableWidth = ImGui::GetContentRegionAvail().x;
        const float32 imageSize = columnCount == 1
            ? availableWidth * SINGLE_LAYER_PREVIEW_SCALE
            : (availableWidth - 2.0f * ImGui::GetStyle().CellPadding.x * static_cast<float32>(columnCount)) / static_cast<float32>(columnCount);

        // A lone layer is drawn smaller, so it is centered
        if (columnCount == 1)
        {
            ImGuiWidgets::BeginHorizontalPadding((availableWidth - imageSize) / 2.0f);
        }

        if (ImGui::BeginTable("##LayersTable", static_cast<int>(columnCount)))
        {
            for (uint32 layerIndex = 0; layerIndex < level.layers.size(); layerIndex++)
            {
                ImGui::TableNextColumn();
                ImGui::PushID(static_cast<int>(layerIndex));

                const ImageLayerData& layer = level.layers[layerIndex];
                const LoadedImage& image = level.images[layerIndex];

                const ImGuiImageInputInfo inputInfo
                {
                    .previewID = layer.preview.has_value() ? std::optional(layer.preview->ID) : std::nullopt,
                    .allowedFileExtensions = INPUT_FILE_EXTENSIONS,
                    .imageWidth = image.width,
                    .imageHeight = image.height,
                    .width = imageSize
                };

                // The widget writes into a copy, so a file that fails to load or does not fit never replaces the layer's current one
                if (Sierra::Path filePath = layer.filePath; ImGuiWidgets::ImageInput("##LayerImageInput", filePath, applicationFrame.platformContext, inputInfo))
                {
                    if (levelIndex == 0 && layerIndex == 0)
                    {
                        // The root layer is the source image, and changing it may resize the levels being drawn, so it goes through the same deferred path as the source input
                        SetInputFilePath(filePath);
                    }
                    else
                    {
                        LoadLayer(applicationFrame, level, levelIndex, layerIndex, filePath);
                    }
                }

                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        if (columnCount == 1)
        {
            ImGuiWidgets::EndPadding();
        }
    }

}
