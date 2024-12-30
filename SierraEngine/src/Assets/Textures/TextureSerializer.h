//
// Created by Nikolay Kanchevski on 16.07.24.
//

#pragma once

#include "../AssetSerializer.h"
#include "TextureAsset.h"

#include "../AssetID.h"
#include "../AssetMetadata.h"

#include "ImageLoader.h"
#include "ImageCompressor.h"

namespace SierraEngine
{

    struct TextureProperties
    {
        TextureType type = TextureType::Undefined;
        TextureFilter filter = TextureFilter::Pixelated;
    };

    struct TextureDetails
    {
        uint32 width = 0;
        uint32 height = 0;

        uint32 levelCount = 0;
        uint32 layerCount = 0;

        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
    };

    enum class TextureCompression : bool
    {
        None,
        BasisUniversal
    };

    struct TextureSerializeInfo
    {
        AssetMetadata metadata = { };
        TextureProperties properties = { };

        TextureCompression compression = TextureCompression::None;
        ImageCompressionLevel compressionLevel = ImageCompressionLevel::Standard;
        ImageQualityLevel qualityLevel = ImageQualityLevel::Standard;

        std::span<const ImageLevel> levels = { };
    };

    struct SerializedTexture
    {
        std::vector<uint8> data = { };
        std::vector<uint8> memory = { };
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
        void SerializeMemory(Sierra::Stream& stream, const TextureSerializeInfo& serializeInfo) const;

        /* --- MOVE SEMANTICS --- */
        TextureSerializer(TextureSerializer&&) noexcept = default;
        TextureSerializer& operator=(TextureSerializer&&) noexcept = default;

    private:
        friend class TextureImporter;
        struct TextureHeader
        {
            TextureCompression compression = { };
        };

    };

}
