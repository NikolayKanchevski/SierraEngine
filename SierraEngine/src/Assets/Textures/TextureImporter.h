//
// Created by Nikolay Kanchevski on 6.11.24.
//

#pragma once

#include "../AssetImporter.h"

#include "TextureSerializer.h"
#include "ImageTranscoder.h"

namespace SierraEngine
{

    struct TextureImportInfo
    {
        const SerializedTexture& serializedTexture;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
    };

    struct ImportedTexture
    {
        AssetHeader header = { };
        AssetMetadata metadata = { };
        TextureProperties properties = { };

        TextureDetails details = { };
        std::vector<uint8> memory = { };
    };

    class SIERRA_ENGINE_API TextureImporter : public virtual AssetImporter
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<ImportedTexture> Import(const TextureImportInfo& importInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        TextureImporter(const TextureImporter&) = delete;
        TextureImporter& operator=(const TextureImporter&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureImporter() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        TextureImporter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] std::optional<TextureDetails> ImportMemory(Sierra::Stream& stream, const TextureImportInfo& importInfo, std::vector<uint8>& memory) const;

        /* --- MOVE SEMANTICS --- */
        TextureImporter(TextureImporter&&) noexcept = default;
        TextureImporter& operator=(TextureImporter&&) noexcept = default;

    };

}