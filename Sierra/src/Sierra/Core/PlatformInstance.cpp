//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include "PlatformInstance.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsInstance.h"
    typedef Sierra::WindowsInstance NativeInstance;
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/LinuxInstance.h"
    typedef Sierra::LinuxInstance NativeInstance;
#elif SR_PLATFORM_MACOS
    #include "Platform/MacOS/MacOSInstance.h"
    typedef Sierra::MacOSInstance NativeInstance;
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSInstance.h"
    typedef Sierra::iOSInstance NativeInstance;
#else
    #error "Platform instance of this system has not been implemented!"
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    PlatformInstance::PlatformInstance(const PlatformInstanceCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    void PlatformInstance::RunApplication(const PlatformApplicationRunInfo &runInfo)
    {
        runInfo.OnStart();
        while (!runInfo.OnUpdate())
        {
            continue;
        }
        runInfo.OnEnd();
    }

    /* --- PRIVATE METHODS --- */

    std::unique_ptr<PlatformInstance> PlatformInstance::Load(const PlatformInstanceCreateInfo &createInfo)
    {
        return std::make_unique<NativeInstance>(createInfo);
    }

}
