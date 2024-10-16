//
// Created by Nikolay Kanchevski on 15.08.23.
//

#include "Window.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Window::Window(const WindowCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.title.empty(), InvalidValueError("Cannot create buffer, as specified title must not be empty"));
        SR_THROW_IF(createInfo.width <= 0, InvalidValueError(SR_FORMAT("Cannot create window [{0}], as specified width must be greater than [{0}]", createInfo.title)));
        SR_THROW_IF(createInfo.height <= 0, InvalidValueError(SR_FORMAT("Cannot create window [{0}], as specified height must be greater than [{0}]", createInfo.title)));
    }

}
