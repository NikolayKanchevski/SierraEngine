//
// Created by Nikolay Kanchevski on 3.07.24.
//

#include "iOSWindowManager.h"

#include "iOSContext.h"
#include "UIKitWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    iOSWindowManager::iOSWindowManager(const WindowManagerCreateInfo &createInfo)
        : WindowManager(createInfo), uiKitContext(static_cast<iOSContext&>(createInfo.platformContext).GetUIKitContext())
    {
        SR_ERROR_IF(createInfo.platformContext.GetType() != PlatformType::iOS, "Cannot create iOS window manager using a platform context of type, which differs from [PlatformType::macOS]!");
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> iOSWindowManager::CreateWindow(const WindowCreateInfo &createInfo) const
    {
        return std::make_unique<UIKitWindow>(uiKitContext, createInfo);
    }

}