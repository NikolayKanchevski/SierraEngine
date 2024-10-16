//
// Created by Nikolay Kanchevski on 16.07.24.
//

#pragma once

#include "../AssetSerializer.h"

#include "ImageCompressor.h"

namespace SierraEngine
{

    struct TextureSerializeInfo
    {
        AssetMetadata metadata = { };

        TextureType type = TextureType::Undefined;
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;

        uint32 levelCount = 0;
        uint32 layerCount = 0;

        Sierra::SamplerFilter filter = Sierra::SamplerFilter::Nearest;
        ImageCompressorType compression = ImageCompressorType::Undefined;
    };

    class TextureSerializer : public AssetSerializer
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<std::vector<uint8>> Serialize(const TextureSerializeInfo& serializeInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        TextureSerializer(const TextureSerializer&) = delete;
        TextureSerializer& operator=(const TextureSerializer&) = delete;

        /* --- MOVE SEMANTICS --- */
        TextureSerializer(TextureSerializer&&) noexcept = default;
        TextureSerializer& operator=(TextureSerializer&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~TextureSerializer() noexcept override = default;

    protected:
        /* --- CONSTRUCTORS --- */
        TextureSerializer() noexcept = default;

    };

}
