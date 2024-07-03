//
// Created by Nikolay Kanchevski on 3.07.24.
//

#include "macOSWindowManager.h"

#include "macOSContext.h"
#include "CocoaWindow.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    macOSWindowManager::macOSWindowManager(const WindowManagerCreateInfo &createInfo)
        : WindowManager(createInfo), cocoaContext(static_cast<macOSContext&>(createInfo.platformContext).GetCocoaContext())
    {
        SR_ERROR_IF(createInfo.platformContext.GetType() != PlatformType::macOS, "Cannot create macOS window manager using a platform context of type, which differs from [PlatformType::macOS]!");
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> macOSWindowManager::CreateWindow(const WindowCreateInfo &createInfo) const
    {
        return std::make_unique<CocoaWindow>(cocoaContext, createInfo);
    }

}