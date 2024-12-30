//
// Created by Nikolay Kanchevski on 5.07.24.
//

#include "TextureAsset.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    TextureAsset::TextureAsset(const TextureCreateInfo& createInfo)
        : image(std::move(createInfo.image)), filter(createInfo.filter)
    {

    }

}