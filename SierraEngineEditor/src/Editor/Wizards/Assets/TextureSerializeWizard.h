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
        const Sierra::Device& device;

        const std::filesystem::path* inputFilePath = nullptr;
        const std::filesystem::path* outputDirectoryPath = nullptr;
    };

    class TextureSerializeWizard final : public FileInputWizard, public AssetSerializeWizard
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureSerializeWizard(const TextureSerializeWizardCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(bool& open, Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) override;

        /* --- CONSTANTS --- */
        constexpr static std::array<std::string_view, 3> INPUT_FILE_EXTENSIONS = { ".jpg", ".jpeg", ".png" };
        constexpr static std::string_view OUTPUT_FILE_EXTENSION = ".texture";

        /* --- COPY SEMANTICS --- */
        TextureSerializeWizard(const TextureSerializeWizard&) = delete;
        TextureSerializeWizard& operator=(const TextureSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        TextureSerializeWizard(TextureSerializeWizard&&) = delete;
        TextureSerializeWizard& operator=(TextureSerializeWizard&&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureSerializeWizard() noexcept override = default;

    private:
        const Sierra::Device& device;

        enum class TextureSerializeFormat : uint8
        {
            YAML
        };

        uint32 textureFormatIndex = 0;
        TextureSerializeInfo serializeInfo = { };

        struct ImageLayerData
        {
            bool dirty = false;
            std::filesystem::path filePath = { };

            std::optional<LoadedImage> image = std::nullopt;
            std::optional<ImagePreview> preview = std::nullopt;
        };

        struct ImageLevelData
        {
            std::vector<ImageLayerData> layers = { };
        };

        enum class LevelOptions : bool
        {
            None,
            Load
        };

        uint32 layerCount = 1;
        LevelOptions levelOptions = LevelOptions::None;

        bool rootLayerDirty = false;
        std::vector<ImageLevelData> levels = { };

        void SetInputFilePath(const std::filesystem::path& inputFilePath) noexcept override;

        [[nodiscard]] ImageLayerData* GetRootLayer() noexcept;
        [[nodiscard]] LoadedImage* GetRootImage() noexcept;
        [[nodiscard]] bool ImageMatchesLevelDimensions(const LoadedImage& image, uint32 levelIndex);

        void ReloadRootLayer(Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) noexcept;
        void ReloadLevels(Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) noexcept;
        void ReloadLayers(uint32 lastLayerCount, Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) noexcept;
        void ResetLayers(Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) noexcept;
        void ResetLayer(ImageLayerData& layer, Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) noexcept;
        bool Serialize() noexcept;

        void DrawPropertiesMenu() noexcept;
        void DrawSerializeInfoMenu() noexcept;
        void DrawExtrasMenu(Sierra::CommandBuffer& level, Sierra::ResourceTable& resourceTable) noexcept;

    };

}
