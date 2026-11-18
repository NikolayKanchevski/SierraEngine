//
// Created by Nikolay Kanchevski on 7.11.24.
//

#pragma once

#include "../FileInputWizard.h"
#include "../AssetSerializeWizard.h"

#include "../../Previews/ImagePreviewer.h"

namespace SierraEngine
{

    struct TextureSerializeWizardCreateInfo
    {
        const Sierra::PlatformContext& platformContext;
        const Sierra::Path* inputFilePath = nullptr;
        const Sierra::Path* outputDirectoryPath = nullptr;
    };

    class TextureSerializeWizard final : public FileInputWizard, public AssetSerializeWizard
    {
    public:
        /* --- CONSTANTS --- */
        constexpr static std::array<std::string_view, 3> INPUT_FILE_EXTENSIONS = { ".jpg", ".jpeg", ".png" };
        constexpr static std::string_view OUTPUT_FILE_EXTENSION = ".texture";

        /* --- CONSTRUCTORS --- */
        explicit TextureSerializeWizard(const TextureSerializeWizardCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame) override;

        /* --- COPY SEMANTICS --- */
        TextureSerializeWizard(const TextureSerializeWizard&) = delete;
        TextureSerializeWizard& operator=(const TextureSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        TextureSerializeWizard(TextureSerializeWizard&&) = delete;
        TextureSerializeWizard& operator=(TextureSerializeWizard&&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureSerializeWizard() noexcept override = default;

    protected:
        /* --- SETTER METHODS --- */
        void SetInputFilePath(const Sierra::Path& inputFilePath) noexcept override;

    private:
        constexpr static uint32 MAX_LAYER_COUNT = 16;

        enum class TextureSerializeFormat : uint8
        {
            YAML
        };

        enum class LevelOptions : bool
        {
            None,
            Load
        };

        struct ImageLayerData
        {
            Sierra::Path filePath = { };
            std::optional<ImagePreview> preview = std::nullopt;
            bool previewOutdated = false;
        };

        struct ImageLevelData
        {
            std::vector<LoadedImage> images = { }; // One per layer, kept contiguous so levels can be handed to the serializer without copying
            std::vector<ImageLayerData> layers = { };
        };

        TextureSerializeInfo serializeInfo = { };
        uint32 textureFormatIndex = 0;

        uint32 layerCount = 1;
        LevelOptions levelOptions = LevelOptions::None;
        std::vector<ImageLevelData> levels = { };

        std::optional<Sierra::Path> pendingInputFilePath = std::nullopt;
        void LoadInputFile(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame, const Sierra::Path& inputFilePath) noexcept;
        void LoadLayer(const ApplicationFrame& applicationFrame, ImageLevelData& level, uint32 levelIndex, uint32 layerIndex, const Sierra::Path& filePath) noexcept;

        [[nodiscard]] const LoadedImage& GetRootImage() const noexcept { return levels.front().images.front(); }
        [[nodiscard]] uint32 GetLevelCount() const noexcept;
        [[nodiscard]] Vector2UInt GetLevelDimensions(uint32 levelIndex) const noexcept;
        [[nodiscard]] bool FitsLevel(const LoadedImage& image, uint32 levelIndex) const noexcept;

        void ResizeLevels(const DrawFrame& drawFrame) noexcept;
        void UpdatePreviews(const DrawFrame& drawFrame) noexcept;
        void FreePreviews(const DrawFrame& drawFrame) noexcept;
        [[nodiscard]] bool Serialize(const ApplicationFrame& applicationFrame) const noexcept;

        void DrawSettingsMenu() noexcept;
        void DrawSerializeInfoMenu() noexcept;
        void DrawExtrasMenu(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame) noexcept;
        void DrawLevel(const ApplicationFrame& applicationFrame, ImageLevelData& level, uint32 levelIndex) noexcept;

    };

}
