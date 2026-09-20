//
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

#include "ImageLoader.h"

namespace SierraEngine
{

    enum class TextureCompressionAggressiveness : uint8
    {
        Lowest,
        Low,
        Standard,
        High,
        Highest
    };

    enum class TextureCompressionQuality : uint8
    {
        Lowest,
        Low,
        Standard,
        High,
        Highest
    };

    struct ImageCompressInfo
    {
        std::span<const LoadedImageLevel> levels = { };
        TextureCompressionAggressiveness compressionLevel = TextureCompressionAggressiveness::Standard;
        TextureCompressionQuality qualityLevel = TextureCompressionQuality::Standard;
    };

    struct CompressedImage
    {
        std::vector<uint8> memory = { };
    };

    class SIERRA_ENGINE_API ImageCompressor
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<CompressedImage> Compress(const ImageCompressInfo& compressInfo) const noexcept = 0;

        /* --- COPY SEMANTICS --- */
        ImageCompressor(const ImageCompressor&) = delete;
        ImageCompressor& operator=(const ImageCompressor&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ImageCompressor() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ImageCompressor() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        ImageCompressor(ImageCompressor&&) noexcept = default;
        ImageCompressor& operator=(ImageCompressor&&) noexcept = default;

    };

}
