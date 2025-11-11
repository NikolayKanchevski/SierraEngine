//
// Created by Nikolay Kanchevski on 24.10.25.
//

#pragma once

#include "../AssetSerializeWizard.h"

namespace SierraEngine
{

    struct MaterialSerializeWizardCreateInfo
    {
        const Sierra::PlatformContext& platformContext;
        const RenderingContext& renderingContext;

        const std::filesystem::path* outputDirectoryPath = nullptr;
    };

    class MaterialSerializeWizard final : public AssetSerializeWizard
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MaterialSerializeWizard(const MaterialSerializeWizardCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        void Draw(bool& open, Sierra::CommandBuffer& commandBuffer) override;

        /* --- CONSTANTS --- */
        constexpr static std::string_view OUTPUT_FILE_EXTENSION = ".material";

        /* --- COPY SEMANTICS --- */
        MaterialSerializeWizard(const MaterialSerializeWizard&) = delete;
        MaterialSerializeWizard& operator=(const MaterialSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        MaterialSerializeWizard(MaterialSerializeWizard&&) noexcept = default;
        MaterialSerializeWizard& operator=(MaterialSerializeWizard&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~MaterialSerializeWizard() noexcept override = default;

    private:
        const RenderingContext* renderingContext;

        enum class MaterialSerializeFormat : uint8
        {
            YAML
        };

        uint32 materialFormatIndex = 0;
        MaterialSerializeInfo serializeInfo = { };

        void DrawPropertiesMenu() noexcept;
        bool Serialize() noexcept;

    };

}