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
        const Sierra::Device& device;

        const std::filesystem::path* outputDirectoryPath = nullptr;
    };

    class MaterialSerializeWizard final : public AssetSerializeWizard
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MaterialSerializeWizard(const MaterialSerializeWizardCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        void Draw(bool& open, Sierra::CommandBuffer& commandBuffer, Sierra::ResourceTable& resourceTable) override;

        /* --- CONSTANTS --- */
        constexpr static std::string_view OUTPUT_FILE_EXTENSION = ".material";

        /* --- COPY SEMANTICS --- */
        MaterialSerializeWizard(const MaterialSerializeWizard&) = delete;
        MaterialSerializeWizard& operator=(const MaterialSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        MaterialSerializeWizard(MaterialSerializeWizard&&) = delete;
        MaterialSerializeWizard& operator=(MaterialSerializeWizard&&) = delete;

        /* --- DESTRUCTOR --- */
        ~MaterialSerializeWizard() noexcept override = default;

    private:
        const Sierra::Device& device;

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