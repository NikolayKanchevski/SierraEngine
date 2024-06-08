//
// Created by Nikolay Kanchevski on 26.09.23.
//

#include "iOSContext.h"

#include <UIKit/UIKit.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    iOSContext::iOSContext(const PlatformContextCreateInfo &createInfo)
        : PlatformContext(createInfo), uiKitContext({ .application = [UIApplication sharedApplication] })
    {
        
    }

}
