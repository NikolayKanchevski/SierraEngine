//
// Created by Nikolay Kanchevski on 10.10.25.
//

#pragma once

#include "ImageLoader.h"

namespace SierraEngine
{

    namespace ImageAccessor
    {
        [[nodiscard]] Color64 ReadPixel(const LoadedImage& image, Vector2UInt coordinate);
        void WritePixel(LoadedImage& image, Vector2UInt coordinate, Color64 color);
    }

}