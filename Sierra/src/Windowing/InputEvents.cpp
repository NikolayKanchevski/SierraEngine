//
// Created by Nikolay Kanchevski on 22.09.24.
//

#include "InputEvents.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    KeyEvent::KeyEvent(const Key key) noexcept
        : key(key)
    {

    }

    /* --- CONSTRUCTORS --- */

    KeyPressEvent::KeyPressEvent(const Key key) noexcept
        : KeyEvent(key)
    {

    }

    /* --- CONSTRUCTORS --- */

    KeyReleaseEvent::KeyReleaseEvent(const Key key) noexcept
        : KeyEvent(key)
    {

    }

    /* --- CONSTRUCTORS --- */

    MouseButtonEvent::MouseButtonEvent(const MouseButton mouseButton) noexcept
        : mouseButton(mouseButton)
    {

    }

    /* --- CONSTRUCTORS --- */

    MouseButtonPressEvent::MouseButtonPressEvent(const MouseButton mouseButton) noexcept
        : MouseButtonEvent(mouseButton)
    {

    }

    /* --- CONSTRUCTORS --- */

    MouseButtonReleaseEvent::MouseButtonReleaseEvent(const MouseButton mouseButton) noexcept
        : MouseButtonEvent(mouseButton)
    {

    }

    /* --- CONSTRUCTORS --- */

    MouseScrollEvent::MouseScrollEvent(const Vector2 scroll) noexcept
        : scroll(scroll)
    {

    }

}