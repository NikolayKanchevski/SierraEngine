//
// Created by Nikolay Kanchevski on 9.2.2023.
//

#include "WindowsContext.h"

#include <shellapi.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    WindowsContext::WindowsContext()
        : win32Context({ .hInstance = GetModuleHandle(nullptr) })
    {

    }

}