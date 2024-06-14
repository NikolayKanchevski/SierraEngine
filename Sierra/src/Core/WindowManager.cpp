//
// Created by Nikolay Kanchevski on 7.11.23.
//

#include "WindowManager.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsContext.h"
    #include "Platform/Windows/Win32Window.h"
    #undef CreateWindow
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/LinuxContext.h"
    #include "Platform/Linux/X11Window.h"
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/macOSContext.h"
    #include "Platform/macOS/CocoaWindow.h"
#elif SR_PLATFORM_ANDROID
    #include "Platform/Android/AndroidContext.h"
    #include "Platform/Android/GameActivityWindow.h"
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSContext.h"
    #include "Platform/iOS/UIKitWindow.h"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowManager::WindowManager(const WindowManagerCreateInfo &createInfo)
        : platformContext(createInfo.platformContext)
    {

    }

    std::unique_ptr<WindowManager> WindowManager::Create(const WindowManagerCreateInfo &createInfo)
    {
        return std::unique_ptr<WindowManager>(new WindowManager(createInfo));
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Window> WindowManager::CreateWindow(const WindowCreateInfo &createInfo) const
    {
        #if SR_PLATFORM_WINDOWS
            SR_ERROR_IF(platformContext.GetType() != PlatformType::Windows, "Cannot create Win32 window using a platform context of type, which differs from [PlatformType::Windows]!");
            return std::make_unique<Win32Window>(static_cast<WindowsContext&>(platformContext).GetWin32Context(), createInfo);
        #elif SR_PLATFORM_LINUX
            SR_ERROR_IF(platformContext.GetType() != PlatformType::Linux, "Cannot create X11 window using a platform context of type, which differs from [PlatformType::Linux]!");
            return std::make_unique<X11Window>(static_cast<const LinuxContext&>(platformContext).GetX11Context(), createInfo);
        #elif SR_PLATFORM_macOS
            SR_ERROR_IF(platformContext.GetType() != PlatformType::macOS, "Cannot create Cocoa window using a platform context of type, which differs from [PlatformType::macOS]!");
            return std::make_unique<CocoaWindow>(static_cast<macOSContext&>(platformContext).GetCocoaContext(), createInfo);
        #elif SR_PLATFORM_ANDROID
            SR_ERROR_IF(platformContext.GetType() != PlatformType::Android, "Cannot create Native window using a platform context of type, which differs from [PlatformType::Android]!");
            return std::make_unique<GameActivityWindow>(static_cast<const AndroidContext&>(platformContext).GetGameActivityContext(), createInfo);
        #elif SR_PLATFORM_iOS
            SR_ERROR_IF(platformContext.GetType() != PlatformType::iOS, "Cannot create UIKit window using a platform context of type, which differs from [PlatformType::iOS]!");
            return std::make_unique<UIKitWindow>(static_cast<iOSContext&>(platformContext).GetUIKitContext(), createInfo);
        #else
            SR_ERROR("Cannot create window on unrecognized operating system!");
            return nullptr;
        #endif
    }

}