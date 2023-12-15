//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include "WindowsContext.h"

#include <shellapi.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowsContext::WindowsContext(const PlatformContextCreateInfo &createInfo)
        : PlatformContext(createInfo), win32Context({ })
    {

    }

}