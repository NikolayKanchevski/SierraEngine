//
// Created by Nikolay Kanchevski on 11.11.26.
//

#pragma once

#include "../FileInputWizard.h"
#include "../AssetSerializeWizard.h"

namespace SierraEngine
{

    struct ModelSerializeWizardCreateInfo
    {
        const EditorPlatformContext& platformContext;
        const RenderingContext& renderingContext;
        const Sierra::FileManager& fileManager;

        const std::filesystem::path* inputFilePath = nullptr;
        const std::filesystem::path* outputDirectoryPath = nullptr;
    };

    class ModelSerializeWizard final : public FileInputWizard, public AssetSerializeWizard
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ModelSerializeWizard(const ModelSerializeWizardCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(bool& open, Sierra::CommandBuffer& commandBuffer) override;

        /* --- CONSTANTS --- */
        constexpr static std::array<std::string_view, 2> INPUT_FILE_EXTENSIONS = { ".glb", ".gltf" };
        constexpr static std::string_view OUTPUT_FILE_EXTENSION = ".model";

        /* --- COPY SEMANTICS --- */
        ModelSerializeWizard(const ModelSerializeWizard&) = delete;
        ModelSerializeWizard& operator=(const ModelSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        ModelSerializeWizard(ModelSerializeWizard&&) noexcept = default;
        ModelSerializeWizard& operator=(ModelSerializeWizard&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ModelSerializeWizard() noexcept override = default;

    private:
        const EditorPlatformContext& platformContext;
        const Sierra::FileManager& fileManager;
        const RenderingContext* renderingContext;

        enum class ModelSerializeFormat : uint8
        {
            YAML
        };

        enum class MeshOptimization : uint8
        {
            None,
            Zeux
        };

        uint32 modelFormatIndex = 0;
        std::optional<LoadedModel> model = std::nullopt;

        // NOTE: Serialize info cannot be stored directly, as it references the loaded model
        AssetMetadata metadata = { };
        ModelSettings settings = { };
        TextureCompressionSettings textureCompressionSettings = { };
        MeshCompressionSettings meshCompressionSettings = { };
        MeshOptimization meshOptimization = MeshOptimization::Zeux;

        void SetInputFilePath(const std::filesystem::path& inputFilePath) noexcept override;
        bool Serialize() noexcept;
        void DrawSerializeInfoMenu() noexcept;

    };

}
