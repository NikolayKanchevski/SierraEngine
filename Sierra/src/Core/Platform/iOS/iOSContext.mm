//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "iOSContext.h"

namespace Sierra
{

    PlatformApplicationRunInfo iOSApplicationRunInfo;
    UIKitContext* iOSUIKitContext;

    /* --- CONSTRUCTORS --- */

    iOSContext::iOSContext(const PlatformContextCreateInfo &createInfo)
        : PlatformContext(createInfo), uiKitContext(UIKitContext({ }))
    {
        
    }

    /* --- POLLING METHODS --- */

    void iOSContext::RunApplication(const PlatformApplicationRunInfo &runInfo)
    {
        iOSApplicationRunInfo = runInfo;
        iOSUIKitContext = &uiKitContext;

        // Run application
        UIApplicationMain(0, nil, nil, @"UIKitApplicationDelegate");
    }

}
