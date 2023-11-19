//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "MacOSInstance.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    MacOSInstance::MacOSInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo), cocoaContext({ })
    {

    }

}