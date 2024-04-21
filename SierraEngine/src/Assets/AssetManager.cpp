//
// Created by Nikolay Kanchevski on 20.02.24.
//

#include "AssetManager.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    AssetManager::AssetManager(const AssetManagerCreateInfo &createInfo)
        : threadPool(createInfo.threadPool), fileManager(createInfo.fileManager), renderingContext(createInfo.renderingContext)
    {

    }

}