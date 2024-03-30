//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "TextureAsset.h"

namespace SierraEngine
{

    struct TextureImporterCreateInfo
    {
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });
    };

    class TextureImporter final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureImporter(const TextureImporterCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<std::pair<TextureAsset, std::unique_ptr<Sierra::Buffer>>> Import(const Sierra::RenderingContext &renderingContext, std::string_view name, const std::pair<const SerializedTexture, const void*> &serializedTexture);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline Sierra::Version GetVersion() const { return version; }

        /* --- OPERATORS --- */
        TextureImporter(const TextureImporter&) = delete;
        TextureImporter &operator=(const TextureImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureImporter() = default;

    private:
        Sierra::Version version = Sierra::Version({ 1, 0, 0 });

    };

}
