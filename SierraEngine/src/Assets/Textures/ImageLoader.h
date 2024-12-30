//
// Created by Nikolay Kanchevski on 11.11.24.
//

#pragma once

#include "Image.h"

namespace SierraEngine
{

    /* --- TYPE DEFINITIONS --- */
    using ImageFormatSignature = std::span<const uint8>;

    enum class ImageLoadChannels : uint8
    {
        R,
        RG,
        RGB,
        RGBA,
        All
    };

    struct ImageLoadInfo
    {
        std::span<const uint8> memory = { };
        ImageLoadChannels loadChannels = ImageLoadChannels::All;
    };

    class SIERRA_ENGINE_API ImageLoader
    {
    public:
        /* --- POLLING METHODS --- */
        [[nodiscard]] virtual std::optional<Image> Load(const ImageLoadInfo& loadInfo) const = 0;

        /* --- COPY SEMANTICS --- */
        ImageLoader(const ImageLoader&) = delete;
        ImageLoader& operator=(const ImageLoader&) = delete;

        /* --- DESTRUCTOR --- */
        ~ImageLoader() noexcept = default;

    protected:
        /* --- CONSTRUCTORS --- */
        ImageLoader() noexcept = default;

        /* --- MOVE SEMANTICS --- */
        ImageLoader(ImageLoader&&) noexcept = default;
        ImageLoader& operator=(ImageLoader&&) noexcept = default;

    };

}