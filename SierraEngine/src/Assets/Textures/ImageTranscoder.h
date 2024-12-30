//
// Created by Nikolay Kanchevski on 18.07.24.
//

#pragma once

namespace SierraEngine
{

    struct ImageTranscodeInfo
    {
        std::span<const uint8> memory = { };
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
    };

    struct TranscodedImage
    {
        uint32 width = 0;
        uint32 height = 0;

        uint32 levelCount = 0;
        uint32 layerCount = 0;

        std::vector<uint8> memory = { };
    };

    class SIERRA_ENGINE_API ImageTranscoder
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<TranscodedImage> Transcode(const ImageTranscodeInfo& transcodeInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        ImageTranscoder(const ImageTranscoder&) = delete;
        ImageTranscoder& operator=(const ImageTranscoder&) = delete;

        /* --- DESTRUCTOR --- */
        virtual ~ImageTranscoder() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ImageTranscoder() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        ImageTranscoder(ImageTranscoder&&) noexcept = default;
        ImageTranscoder& operator=(ImageTranscoder&&) noexcept = default;

    };

}
