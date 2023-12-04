//
// Created by Nikolay Kanchevski on 3.09.23.
//

#include "macOSInstance.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    macOSInstance::macOSInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo), cocoaContext({ })
    {

    }

}