//
// Created by Nikolay Kanchevski on 7.11.24.
//

#pragma once

#include "../EditorWizard.h"

//#include "../../Assets/Textures/STBImage.h"
//#include "../../Assets/Textures/TextureSerializer.h"

namespace SierraEngine
{

    struct TextureSerializeWizardCreateInfo
    {
        const Sierra::PlatformContext& platformContext;
        const std::filesystem::path& selectedFilePath = { };
    };

    class TextureSerializeWizard final : public EditorWizard
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureSerializeWizard(const TextureSerializeWizardCreateInfo& createInfo);

        /* --- POLLING METHODS --- */
        void Draw(bool& open) override;

        /* --- COPY SEMANTICS --- */
        TextureSerializeWizard(const TextureSerializeWizard&) = delete;
        TextureSerializeWizard& operator=(const TextureSerializeWizard&) = delete;

        /* --- MOVE SEMANTICS --- */
        TextureSerializeWizard(TextureSerializeWizard&&) noexcept = default;
        TextureSerializeWizard& operator=(TextureSerializeWizard&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~TextureSerializeWizard() noexcept override = default;

    private:
        const Sierra::PlatformContext* platformContext;

        std::filesystem::path sourceFilePath = { };
        std::filesystem::path outputFilePath = { };
        TextureSerializeInfo serializeInfo = { };

        bool SelectSourceFile();
        void ApplyDefaults();
        bool SelectOutputFile();

    };

}