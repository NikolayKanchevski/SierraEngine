//
// Created by Nikolay Kanchevski on 20.02.24.
//

#pragma once

#include "../SerializedTexture.h"

namespace SierraEngine
{

    struct TextureImporterCreateInfo
    {

    };

    struct TextureImportInfo
    {
        std::string_view name = "Imported Texture";
        const Sierra::RenderingContext &renderingContext;
    };

    struct ImportedTexture
    {
        TextureType type = TextureType::Undefined;
        std::unique_ptr<Sierra::Image> image = nullptr;
        std::vector<std::unique_ptr<Sierra::Buffer>> levelBuffers;
    };

    class TextureImporter final
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit TextureImporter(const TextureImporterCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<ImportedTexture> Import(const TextureImportInfo &importInfo, std::span<const uint8> serializedTextureMemory) const;

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
