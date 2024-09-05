//
// Created by Nikolay Kanchevski on 20.07.24.
//

#include "Image.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Image::Image(const ImageCreateInfo& createInfo)
    {
        APP_ERROR_IF(createInfo.compressedMemory.empty(), "Cannot create an image with empty compressed memory!");
    }

}