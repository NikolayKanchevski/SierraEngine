//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include "PlatformContext.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsContext.h"
    typedef Sierra::WindowsContext NativeInstance;
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/LinuxContext.h"
    typedef Sierra::LinuxContext NativeInstance;
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/macOSContext.h"
    typedef Sierra::macOSContext NativeInstance;
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSContext.h"
    typedef Sierra::iOSContext NativeInstance;
#elif SR_PLATFORM_ANDROID
    #include "Platform/Android/AndroidContext.h"
    typedef Sierra::AndroidContext NativeInstance;
#endif

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    PlatformContext::PlatformContext(const PlatformContextCreateInfo &createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    void PlatformContext::RunApplication(const PlatformApplicationRunInfo &runInfo)
    {
        runInfo.OnStart();
        while (!runInfo.OnUpdate())
        {
            continue;
        }
        runInfo.OnEnd();
    }

    /* --- PRIVATE METHODS --- */

    std::unique_ptr<PlatformContext> PlatformContext::Load(const PlatformContextCreateInfo &createInfo)
    {
        return std::make_unique<NativeInstance>(createInfo);
    }

}
