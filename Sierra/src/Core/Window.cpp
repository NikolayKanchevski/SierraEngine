//
// Created by Nikolay Kanchevski on 15.08.23.
//

#include "Window.h"

namespace Sierra
{

    namespace
    {
        struct NullInputManager final : public InputManager { };
        struct NullCursorManager final : public CursorManager { };
        struct NullTouchManager final : public TouchManager { };
    }

    /* --- CONSTRUCTORS --- */

    Window::Window(const WindowCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.title.empty(), "Window title must not be empty!");
    }

    /* --- GETTER METHODS --- */

    InputManager& Window::GetInputManager()
    {
        static NullInputManager nullInputManger = NullInputManager();
        return nullInputManger;
    }

    CursorManager& Window::GetCursorManager()
    {
        static NullCursorManager nullCursorManager = NullCursorManager();
        return nullCursorManager;
    }

    TouchManager& Window::GetTouchManager()
    {
        static NullTouchManager nullTouchManager = NullTouchManager();
        return nullTouchManager;
    }

}
