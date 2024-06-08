//
// Created by Nikolay Kanchevski on 3.10.23.
//

#include "TouchManager.h"

namespace Sierra
{

    /* --- GETTER METHODS --- */

    std::span<const Touch> TouchManager::GetTouches() const
    {
        return { };
    }

    /* --- POLLING METHODS --- */

    void TouchManager::RegisterTouchPress(const Touch&)
    {

    }

    void TouchManager::RegisterTouchMove(const uint64, const Vector2)
    {

    }

    void TouchManager::RegisterTouchRelease(const uint64)
    {

    }

}
