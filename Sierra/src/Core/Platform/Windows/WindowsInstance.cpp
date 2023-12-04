//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include "WindowsInstance.h"

#include <shellapi.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowsInstance::WindowsInstance(const PlatformInstanceCreateInfo &createInfo)
        : PlatformInstance(createInfo), win32Context({ .processID = GetCurrentProcessId() })
    {

    }

}