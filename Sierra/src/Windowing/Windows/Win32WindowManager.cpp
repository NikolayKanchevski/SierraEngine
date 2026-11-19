//
// Created by Nikolay Kanchevski on 19.11.26.
//

#include "Win32WindowManager.h"

#include "Win32Window.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Win32WindowManager::Win32WindowManager(const Win32WindowManagerCreateInfo& createInfo)
        : win32Context(createInfo.win32Context)
    {

    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> Win32WindowManager::CreateWindow(const WindowCreateInfo& createInfo) const noexcept
    {
        return std::make_unique<Win32Window>(win32Context, createInfo);
    }

    void Win32WindowManager::EnumerateScreens(const ScreenEnumerationPredicate& Predicate) const noexcept
    {
        for (const Win32Screen& screen : win32Context.GetScreens())
        {
            Predicate(screen);
        }
    }

    /* --- GETTER METHODS --- */

    const Screen& Win32WindowManager::GetWindowScreen(const Window& window) const
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Win32, UnexpectedTypeError(SR_FORMAT("Cannot get screen of window [{0}], as its windowing backend differs from [WindowingBackendType::Win32]", window.GetTitle())));
        const Win32Window& win32Window = static_cast<const Win32Window&>(window);

        return win32Context.GetWindowScreen(win32Window.GetHwnd());
    }

}
