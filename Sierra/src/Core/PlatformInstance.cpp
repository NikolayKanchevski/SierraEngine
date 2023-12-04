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
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/macOSInstance.h"
    typedef Sierra::macOSInstance NativeInstance;
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSInstance.h"
    typedef Sierra::iOSInstance NativeInstance;
#elif SR_PLATFORM_ANDROID
    #include "Platform/Android/AndroidInstance.h"
    typedef Sierra::AndroidInstance NativeInstance;
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
