//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "iOSInstance.h"

#include "UIKitTemporaryCreateInfoStorage.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    iOSInstance::iOSInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo), uiKitContext(UIKitContext({ }))
    {
        
    }

    /* --- POLLING METHODS --- */

    void iOSInstance::RunApplication(const PlatformApplicationRunInfo &runInfo)
    {
        // Save current run info
        Sierra::UIKitTemporaryCreateInfoStorage::Push({ .context = &uiKitContext, .runInfo = runInfo });

        // Run application
        char** argv = nil; // Gets rid of compiler warning
        UIApplicationMain(0, argv, nil, @"UIKitApplicationDelegate");
    }

}
