//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "TouchManager.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    TouchManager::TouchManager(const TouchManagerCreateInfo &createInfo)
    {

    }

    /* --- GETTER METHODS --- */

    uint32 TouchManager::GetTouchCount()
    {
        return 0;
    }

    std::optional<Touch> TouchManager::GetTouch(const uint32 touchIndex)
    {
        return std::nullopt;
    }

}
