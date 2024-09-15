//
// Created by Nikolay Kanchevski on 8.7.2024.
//

#include "WindowsWindowManager.h"

#include "WindowsContext.h"
#include "Win32Window.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowsWindowManager::WindowsWindowManager(const WindowManagerCreateInfo &createInfo)
        : WindowManager(createInfo), win32Context(static_cast<WindowsContext&>(createInfo.platformContext).GetWin32Context())
    {
        SR_ERROR_IF(createInfo.platformContext.GetType() != PlatformType::Windows, "Cannot create Windows window manager using a platform context of type, which differs from [PlatformType::Windows]!");
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> WindowsWindowManager::CreateWindow(const WindowCreateInfo &createInfo) const
    {
        return std::make_unique<Win32Window>(win32Context, createInfo);
    }

}