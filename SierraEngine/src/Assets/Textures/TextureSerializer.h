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

    struct TextureProperties
    {
        TextureFilter filter = TextureFilter::Smooth;
    };

    enum class ImageCompression : uint8
    {
        None,
        BasisUniversal
    };

    struct TextureSerializeInfo
    {
        AssetMetadata metadata = { };
        TextureProperties properties = { };

        ImageCompression compression = ImageCompression::None;
        ImageCompressionLevel compressionLevel = ImageCompressionLevel::Standard;
        ImageCompressionQualityLevel compressionQualityLevel = ImageCompressionQualityLevel::Standard;
        std::span<const LoadedImageLevel> levels = { };
    };

    struct TextureHeader
    {
        uint32 width = 0;
        uint32 height = 0;

        uint32 levelCount = 0;
        uint32 layerCount = 0;

        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        ImageCompression compression = ImageCompression::None;
    };

    struct SerializedTexture
    {
        std::vector<uint8> data = { };
        std::vector<uint8> blob = { };
    };

    class SIERRA_ENGINE_API TextureSerializer : public virtual AssetSerializer
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<SerializedTexture> Serialize(const TextureSerializeInfo& serializeInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        TextureSerializer(const TextureSerializer&) = delete;
        TextureSerializer& operator=(const TextureSerializer&) = delete;

        /* --- DESTRUCTOR --- */
        ~TextureSerializer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        TextureSerializer() noexcept = default;

        /* --- POLLING METHODS --- */
        void SerializeBlob(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo) const;

        /* --- GETTER METHODS --- */
        [[nodiscard]] size GetTextureMemorySize(const TextureSerializeInfo& serializeInfo) const noexcept;

        /* --- MOVE SEMANTICS --- */
        TextureSerializer(TextureSerializer&&) noexcept = default;
        TextureSerializer& operator=(TextureSerializer&&) noexcept = default;

    };

}
