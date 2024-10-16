//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include "WindowsContext.h"

#include "../../Windowing/Windows/Win32Window.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowsContext::WindowsContext()
        : win32Context({ .hInstance = GetModuleHandle(nullptr) }), fileManager()
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> WindowsContext::CreateWindow(const WindowCreateInfo& createInfo) const
    {
        return std::make_unique<Win32Window>(win32Context, createInfo);
    }

    /* --- GETTER METHODS --- */

    Screen& WindowsContext::GetWindowScreen(const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Win32, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::Win32]", window.GetTitle())));
        const Win32Window& win32Window = static_cast<const Win32Window&>(window);

        return win32Context.GetWindowScreen(win32Window.GetHwnd());
    }

    void WindowsContext::EnumerateScreens(const ScreenEnumerationPredicate& Predicate)
    {
        for (Win32Screen& screen : win32Context.GetScreens())
        {
            Predicate(screen);
        }
    }

    /* --- PRIVATE METHODS --- */

    void WindowsContext::Update()
    {
        win32Context.Update();
    }

}