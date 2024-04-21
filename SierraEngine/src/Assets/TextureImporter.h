//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "TextureAsset.h"

namespace SierraEngine
{

    struct TextureImporterCreateInfo
    {

    };

    struct ImportedTexture
    {
        TextureAsset texture;
        std::vector<std::unique_ptr<Sierra::Buffer>> levelBuffers;
    };

    class TextureImporter final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureImporter(const TextureImporterCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedTexture> Import(const Sierra::RenderingContext &renderingContext, std::string_view name, const SerializedTexture &serializedTexture, std::span<const uint8> blob) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Sierra::Version GetVersion() const { return VERSION; }

        /* --- OPERATORS --- */
        TextureImporter(const TextureImporter&) = delete;
        TextureImporter& operator=(const TextureImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureImporter() = default;

    private:
        constexpr static Sierra::Version VERSION = Sierra::Version({ 1, 0, 0 });

    };

}
