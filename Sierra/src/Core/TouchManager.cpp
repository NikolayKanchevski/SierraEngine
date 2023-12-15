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

    uint32 TouchManager::GetTouchCount() const
    {
        return 0;
    }

    const Touch& TouchManager::GetTouch(const uint32 touchIndex) const
    {
        SR_ERROR("Touch index [{0}] out of range! Make sure to use TouchManager::GetTouchCount() and retrieve touches within the returned range.", touchIndex);

        static Touch touch = Touch({ });
        return touch;
    }

}
