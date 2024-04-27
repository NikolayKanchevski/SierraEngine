//
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

#include "../../Platform/Editor/Serializers/Processors/ImageSupercompressor.h"

namespace SierraEngine
{

    struct ImageTranscoderCreateInfo
    {

    };

    struct ImageTranscodeInfo
    {
        std::span<const uint8> compressedMemory;
    };

    struct TranscodedImage
    {
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        std::vector<std::vector<uint8>> levelMemories = { };
        uint32 layerCount = 0;
    };

    class ImageTranscoder
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<TranscodedImage> Transcode(const ImageTranscodeInfo &transcodeInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual ImageSupercompressorType GetType() const = 0;

        /* --- OPERATORS --- */
        ImageTranscoder(const ImageTranscoder&) = delete;
        ImageTranscoder& operator=(const ImageTranscoder&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ImageTranscoder() = default;

    protected:
        explicit ImageTranscoder(const ImageTranscoderCreateInfo &createInfo);

    };

}
