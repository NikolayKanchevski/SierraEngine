//
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

#include "Image.h"

namespace SierraEngine
{

    enum class ImageCompressorType : bool
    {
        Undefined,
        BasisUniversal
    };

    enum class ImageCompressionLevel : uint8
    {
        Lowest,
        Low,
        Standard,
        High,
        Highest
    };

    enum class ImageQualityLevel : uint8
    {
        Lowest,
        Low,
        Standard,
        High,
        Highest
    };

    struct ImageCompressInfo
    {
        std::span<const ImageLevel> imageLevels = { };
        ImageCompressionLevel compressionLevel = ImageCompressionLevel::Standard;
        ImageQualityLevel qualityLevel = ImageQualityLevel::Standard;
    };

    class ImageCompressor
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<std::vector<uint8>> Compress(const ImageCompressInfo& compressInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual ImageCompressorType GetType() const = 0;

        /* --- COPY SEMANTICS --- */
        ImageCompressor(const ImageCompressor&) = delete;
        ImageCompressor& operator=(const ImageCompressor&) = delete;

        /* --- MOVE SEMANTICS --- */
        ImageCompressor(ImageCompressor&&) = default;
        ImageCompressor& operator=(ImageCompressor&&) = default;

        /* --- DESTRUCTOR --- */
        virtual ~ImageCompressor() = default;

    protected:
        ImageCompressor() = default;

    };

}
