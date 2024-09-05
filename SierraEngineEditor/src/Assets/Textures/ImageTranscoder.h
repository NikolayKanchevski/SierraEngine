//
// Created by Nikolay Kanchevski on 18.07.24.
//

#pragma once

namespace SierraEngine
{

    enum class ImageTranscoderType : bool
    {
        Undefined,
        BasisUniversal
    };

    struct ImageTranscodeInfo
    {
        uint32 width = 0;
        uint32 height = 0;
        std::span<const uint8> memory = { };

        uint32 levelCount = 1;
        uint32 layerCount = 1;

        Sierra::ImageFormat transcodeFormat = Sierra::ImageFormat::Undefined;
    };

    class ImageTranscoder
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<std::vector<uint8>> Transcode(const ImageTranscodeInfo& transcodeInfo) const = 0;

        /* --- GETTER METHODS --- */
        [[nodiscard]] virtual ImageTranscoderType GetType() const = 0;

        /* --- COPY SEMANTICS --- */
        ImageTranscoder(const ImageTranscoder&) = delete;
        ImageTranscoder& operator=(const ImageTranscoder&) = delete;

        /* --- MOVE SEMANTICS --- */
        ImageTranscoder(ImageTranscoder&&) = default;
        ImageTranscoder& operator=(ImageTranscoder&&) = default;

        /* --- DESTRUCTOR --- */
        virtual ~ImageTranscoder() = default;

    protected:
        ImageTranscoder() = default;

    };

}
