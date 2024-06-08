//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "macOSContext.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    macOSContext::macOSContext(const PlatformContextCreateInfo &createInfo)
        : PlatformContext(createInfo), cocoaContext({ .application = [NSApplication sharedApplication] })
    {

    }

}