//
// Created by Nikolay Kanchevski on 7.11.23.
//

#include "WindowManager.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsInstance.h"
    #include "Platform/Windows/Win32Window.h"
    #undef CreateWindow
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/X11Window.h"
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/macOSInstance.h"
    #include "Platform/macOS/CocoaWindow.h"
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSInstance.h"
    #include "Platform/iOS/UIKitWindow.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    std::unique_ptr<WindowManager> WindowManager::Create(const WindowManagerCreateInfo &createInfo)
    {
        return std::unique_ptr<WindowManager>(new WindowManager(createInfo));
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> WindowManager::CreateWindow(const WindowCreateInfo &createInfo) const
    {
        #if SR_PLATFORM_WINDOWS
            SR_ERROR_IF(platformInstance->GetType() !=+ PlatformType::Windows, "Cannot create Win32 window using a platform instance of type [{0}] when it must be [{1}]!", platformInstance->GetType()._to_string(), PlatformType(PlatformType::Windows)._to_string());
            return std::make_unique<Win32Window>(static_cast<const WindowsInstance*>(platformInstance.get())->GetWin32Context(), createInfo);
        #elif SR_PLATFORM_LINUX
            #error "Unimplemented!"
        #elif SR_PLATFORM_macOS
            SR_ERROR_IF(platformInstance->GetType() !=+ PlatformType::macOS, "Cannot create Cocoa window using a platform instance of type [{0}] when it must be [{1}]!", platformInstance->GetType()._to_string(), PlatformType(PlatformType::macOS)._to_string());
            return std::make_unique<CocoaWindow>(static_cast<const macOSInstance*>(platformInstance.get())->GetCocoaContext(), createInfo);
        #elif SR_PLATFORM_iOS
            SR_ERROR_IF(platformInstance->GetType() !=+ PlatformType::iOS, "Cannot create UIKit window using a platform instance of type [{0}] when it must be [{1}]!", platformInstance->GetType()._to_string(), PlatformType(PlatformType::iOS)._to_string());
            return std::make_unique<UIKitWindow>(static_cast<const iOSInstance*>(platformInstance.get())->GetUIKitContext(), createInfo);
        #else
            SR_ERROR("Cannot create window on unrecognized operating system!");
            return nullptr;
        #endif
    }

    /* --- PRIVATE METHODS --- */

    WindowManager::WindowManager(const WindowManagerCreateInfo &createInfo)
        : platformInstance(createInfo.platformInstance)
    {

    }

}