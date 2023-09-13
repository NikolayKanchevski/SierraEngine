//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include "PlatformInstance.h"

#if PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsInstance.h"
    typedef Sierra::WindowsInstance NativeInstance;

#elif PLATFORM_LINUX
    #include "Platform/Linux/LinuxInstance.h"
    typedef Sierra::LinuxInstance NativeInstance;

#elif PLATFORM_MACOS
    #include "Platform/MacOS/MacOSInstance.h"
    typedef Sierra::MacOSInstance NativeInstance;

#else
    typedef Sierra::PlatformInstance NativeInstance;

#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    PlatformInstance::PlatformInstance(const PlatformInstanceCreateInfo &createInfo)
    {

    }

    UniquePtr<PlatformInstance> PlatformInstance::Create(const PlatformInstanceCreateInfo &createInfo)
    {
        return std::make_unique<NativeInstance>(createInfo);
    }

}
