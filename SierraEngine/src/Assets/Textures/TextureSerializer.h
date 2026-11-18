//
// Created by Nikolay Kanchevski on 16.07.24.
//

#pragma once

#include "../AssetSerializer.h"
#include "../AssetMetadata.h"

#include "ImageLoader.h"
#include "ImageCompressor.h"

namespace SierraEngine
{

    enum class TextureFilter
    {
        Pixelated,
        Smooth
    };

    struct TextureSettings
    {
        TextureFilter filter = TextureFilter::Smooth;
    };

    enum class TextureCompression : uint8
    {
        None,
        BasisUniversal
    };

    struct TextureCompressionSettings
    {
        TextureCompression compression = TextureCompression::None;
        TextureCompressionAggressiveness aggressiveness = TextureCompressionAggressiveness::Standard;
        TextureCompressionQuality quality = TextureCompressionQuality::Standard;
    };

    struct TextureSerializeInfo
    {
        AssetMetadata metadata = { };
        TextureSettings settings = { };

        TextureCompressionSettings compressionSettings = { };
        std::span<const LoadedImageLevel> levels = { };
    };

    using SerializedTexture = SerializedAsset;

    class SIERRA_ENGINE_API TextureSerializer : public virtual AssetSerializer
    {
    public:
        /* --- CONSTANTS METHODS --- */
        constexpr static AssetTypeSignature TYPE_SIGNATURE = { 'T', 'X' };

        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<SerializedTexture> Serialize(const TextureSerializeInfo& serializeInfo, TextureID& outID) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] AssetTypeSignature GetTypeSignature() const noexcept override { return TYPE_SIGNATURE; }

        /* --- COPY SEMANTICS --- */
        TextureSerializer(const TextureSerializer&) = delete;
        TextureSerializer& operator=(const TextureSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureSerializer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        TextureSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] Sierra::StreamResult SerializeBlob(Sierra::Stream& blob, const TextureSerializeInfo& serializeInfo) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetTextureMemorySize(const TextureSerializeInfo& serializeInfo) const noexcept;

        /* --- MOVE SEMANTICS --- */
        TextureSerializer(TextureSerializer&&) noexcept = default;
        TextureSerializer& operator=(TextureSerializer&&) noexcept = default;
    };

}
