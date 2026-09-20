//
// Created by Nikolay Kanchevski on 6.11.24.
//

#pragma once

#include "../AssetImporter.h"

#include "TextureSerializer.h"

namespace SierraEngine
{

    struct TextureImportInfo
    {
        const SerializedTexture& serializedTexture;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
    };

    using TextureFormat = Sierra::ImageFormat;
    struct TextureProperties
    {
        uint32 width = 0;
        uint32 height = 0;

        uint32 levelCount = 0;
        uint32 layerCount = 0;

        TextureFormat format = TextureFormat::Undefined;
        TextureCompression compression = TextureCompression::None;
    };

    struct ImportedTextureBuffer
    {
        std::vector<uint8> memory = { };
    };

    struct ImportedTexture
    {
        TextureID ID = { };
        AssetHeader header = { };
        AssetMetadata metadata = { };
        TextureSettings settings = { };

        TextureProperties properties = { };
        ImportedTextureBuffer buffer = { };
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
        [[nodiscard]] std::optional<ImportedTextureBuffer> ImportBlob(Sierra::Stream& blob, Sierra::ImageFormat format, const TextureProperties& properties) const;

        /* --- MOVE SEMANTICS --- */
        TextureImporter(TextureImporter&&) noexcept = default;
        TextureImporter& operator=(TextureImporter&&) noexcept = default;

    };

}