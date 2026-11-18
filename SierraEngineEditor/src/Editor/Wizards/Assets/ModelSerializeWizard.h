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
        const Sierra::PlatformContext& platformContext;
        const Sierra::Path* inputFilePath = nullptr;
        const Sierra::Path* outputDirectoryPath = nullptr;
    };

    class ModelSerializeWizard final : public FileInputWizard, public AssetSerializeWizard
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit ModelSerializeWizard(const ModelSerializeWizardCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame) override;

        /* --- CONSTANTS --- */
        constexpr static std::array<std::string_view, 2> INPUT_FILE_EXTENSIONS = { ".glb", ".gltf" };
        constexpr static std::string_view OUTPUT_FILE_EXTENSION = ".model";

        /* --- COPY SEMANTICS --- */
        ModelSerializeWizard(const ModelSerializeWizard&) = delete;
        ModelSerializeWizard& operator=(const ModelSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        ModelSerializeWizard(ModelSerializeWizard&&) = delete;
        ModelSerializeWizard& operator=(ModelSerializeWizard&&) = delete;

        /* --- DESTRUCTOR --- */
        ~ModelSerializeWizard() noexcept override = default;

    private:
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

        void SetInputFilePath(const Sierra::PlatformContext& platformContext, const Sierra::Path& inputFilePath) noexcept;
        bool Serialize(const ApplicationFrame& applicationFrame) noexcept;
        void DrawSerializeInfoMenu() noexcept;

    };

}
