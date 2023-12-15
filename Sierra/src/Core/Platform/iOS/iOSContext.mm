//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "iOSContext.h"

#include "UIKitTemporaryCreateInfoStorage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    iOSContext::iOSContext(const PlatformContextCreateInfo &createInfo)
        : PlatformContext(createInfo), uiKitContext(UIKitContext({ }))
    {
        
    }

    /* --- POLLING METHODS --- */

    void iOSContext::RunApplication(const PlatformApplicationRunInfo &runInfo)
    {
        // Save current run info
        Sierra::UIKitTemporaryCreateInfoStorage::Push({ .context = &uiKitContext, .runInfo = runInfo });

        // Run application
        char** argv = nil; // Gets rid of compiler warning
        UIApplicationMain(0, argv, nil, @"UIKitApplicationDelegate");
    }

}
