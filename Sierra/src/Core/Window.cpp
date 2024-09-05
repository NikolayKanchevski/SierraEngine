//
// Created by Nikolay Kanchevski on 15.08.23.
//

#include "Window.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Window::Window(const WindowCreateInfo& createInfo)
    {
        SR_ERROR_IF(createInfo.title.empty(), "Window title must not be empty!");
    }

}
