//
// Created by Nikolay Kanchevski on 18.02.24.
//

#include "Application.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    Application::Application(const ApplicationCreateInfo& createInfo)
        : Sierra::Application({ .name = createInfo.name, .version = createInfo.version, .settings = createInfo.settings })
    {

    }

}
