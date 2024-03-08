//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include "PlatformContext.h"

#if SR_PLATFORM_WINDOWS
    #include "Platform/Windows/WindowsContext.h"
    using NativeInstance = Sierra::WindowsContext;
#elif SR_PLATFORM_LINUX
    #include "Platform/Linux/LinuxContext.h"
    using NativeInstance = Sierra::LinuxContext;
#elif SR_PLATFORM_macOS
    #include "Platform/macOS/macOSContext.h"
    using NativeInstance = Sierra::macOSContext;
#elif SR_PLATFORM_iOS
    #include "Platform/iOS/iOSContext.h"
    using NativeInstance = Sierra::iOSContext;
#elif SR_PLATFORM_ANDROID
    #include "Platform/Android/AndroidContext.h"
    using NativeInstance = Sierra::AndroidContext;
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
        while (!runInfo.OnUpdate());
        runInfo.OnEnd();
    }

    /* --- PRIVATE METHODS --- */

    std::unique_ptr<PlatformContext> PlatformContext::Create(const PlatformContextCreateInfo &createInfo)
    {
        return std::make_unique<NativeInstance>(createInfo);
    }

}
