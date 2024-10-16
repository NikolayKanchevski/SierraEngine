//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "TouchEvents.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    TouchEvent::TouchEvent(const Touch& touch) noexcept
        : touch(touch)
    {

    }

    /* --- CONSTRUCTORS --- */

    TouchPressEvent::TouchPressEvent(const Touch& touch) noexcept
        : TouchEvent(touch)
    {

    }

    /* --- CONSTRUCTORS --- */

    TouchMoveEvent::TouchMoveEvent(const Touch& touch) noexcept
        : TouchEvent(touch)
    {

    }

    /* --- CONSTRUCTORS --- */

    TouchReleaseEvent::TouchReleaseEvent(const Touch& touch) noexcept
        : TouchEvent(touch)
    {

    }

}