//
// Created by Nikolay Kanchevski on 15.08.23.
//

#include "Window.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/Win32Window.h"
    typedef Sierra::Win32Window NativeWindow;
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/X11Window.h"
    typedef Sierra::X11Window NativeWindow;
#elif SR_PLATFORM_MACOS
    #include "Platform/MacOS/CocoaWindow.h"
    typedef Sierra::CocoaWindow NativeWindow;
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/UIKitWindow.h"
    typedef Sierra::UIKitWindow NativeWindow;
#else
    #error "Windowing support for this system has not been implemented!"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Window::Window(const WindowCreateInfo &createInfo)
    {
        SR_ERROR_IF(createInfo.title.empty(), "Window title must not be empty!");
    }

    UniquePtr<Window> Window::Create(const WindowCreateInfo &createInfo)
    {
        PROFILE_SCOPE();
        return std::make_unique<NativeWindow>(createInfo);
    }

    /* --- GETTER METHODS --- */

    InputManager& Window::GetInputManager()
    {
        static InputManager nullInputManger = InputManager({ });
        return nullInputManger;
    }

    CursorManager& Window::GetCursorManager()
    {
        static CursorManager nullCursorManager = CursorManager({ });
        return nullCursorManager;
    }

    TouchManager& Window::GetTouchManager()
    {
        static TouchManager nullTouchManager = TouchManager({ });
        return nullTouchManager;
    }

}
