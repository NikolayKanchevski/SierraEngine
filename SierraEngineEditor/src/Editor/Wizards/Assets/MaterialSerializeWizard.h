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
        const Sierra::Path* outputDirectoryPath = nullptr;
    };

    class MaterialSerializeWizard final : public AssetSerializeWizard
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit MaterialSerializeWizard(const MaterialSerializeWizardCreateInfo& createInfo) noexcept;

        /* --- POLLING METHODS --- */
        [[nodiscard]] bool Draw(const ApplicationFrame& applicationFrame, const DrawFrame& drawFrame) override;

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
        enum class MaterialSerializeFormat : uint8
        {
            YAML
        };

        uint32 materialFormatIndex = 0;
        MaterialSerializeInfo serializeInfo = { };

        void DrawSettingsMenu() noexcept;
        bool Serialize(const ApplicationFrame& applicationFrame) noexcept;

    };

}