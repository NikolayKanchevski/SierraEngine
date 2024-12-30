//
// Created by Nikolay Kanchevski on 19.11.24.
//

#include "MaterialAsset.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    MaterialAsset::MaterialAsset(const MaterialCreateInfo& createInfo)
        : alphaMode(createInfo.alphaMode), cullMode(createInfo.cullMode)
    {

    }

}