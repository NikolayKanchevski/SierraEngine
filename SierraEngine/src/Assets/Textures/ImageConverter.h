//
// Created by Nikolay Kanchevski on 29.12.24.
//

#pragma once

#include "ImageLoader.h"

namespace SierraEngine
{

    struct ImageConvertInfo
    {
        const LoadedImage& image;
        Sierra::ImageFormat format = Sierra::ImageFormat::Undefined;
    };

    class ImageConverter
    {
    public:
        /* --- CONSTRUCTORS --- */
        ImageConverter() noexcept = default;

        /* --- POLLING METHODS --- */
        [[nodiscard]] LoadedImage Convert(const ImageConvertInfo& convertInfo) const;

        /* --- COPY SEMANTICS --- */
        ImageConverter(const ImageConverter&) = delete;
        ImageConverter& operator=(const ImageConverter&) = delete;

        /* --- MOVE SEMANTICS --- */
        ImageConverter(ImageConverter&&) noexcept = default;
        ImageConverter& operator=(ImageConverter&&) noexcept = default;

        /* --- DESTRUCTOR --- */
        ~ImageConverter() noexcept = default;
    };

}
