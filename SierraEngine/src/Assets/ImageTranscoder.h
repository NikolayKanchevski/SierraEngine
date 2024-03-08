//
// Created by Nikolay Kanchevski on 3.03.24.
//

#pragma once

#include "ImageSupercompressor.h"

namespace SierraEngine
{

    struct ImageTranscoderCreateInfo
    {

    };

    struct ImageTranscodeInfo
    {
        const void* compressedMemory = nullptr;
        uint64 compressedMemorySize = 0;
    };

    struct TranscodedImage
    {
        uint32 width = 0;
        uint32 height = 0;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
        uint64 memorySize = 0;
    };

    class ImageTranscoder
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<TranscodedImage> Transcode(const ImageTranscodeInfo &transcodeInfo, void*& transcodedMemory) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual ImageSupercompressorType GetType() const = 0;

        /* --- OPERATORS --- */
        ImageTranscoder(const ImageTranscoder&) = delete;
        ImageTranscoder &operator=(const ImageTranscoder&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ImageTranscoder() = default;

    protected:
        explicit ImageTranscoder(const ImageTranscoderCreateInfo &createInfo);

    };

}
