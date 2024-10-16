//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "WindowEvents.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowResizeEvent::WindowResizeEvent(const uint32 width, const uint32 height) noexcept
        : width(width), height(height)
    {

    }

    /* --- CONSTRUCTORS --- */

    WindowMoveEvent::WindowMoveEvent(const Vector2Int position) noexcept
        : position(position)
    {

    }

    /* --- CONSTRUCTORS --- */

    WindowFocusEvent::WindowFocusEvent(const bool focused) noexcept
        : focused(focused)
    {

    }

}