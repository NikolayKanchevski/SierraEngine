//
// Created by Nikolay Kanchevski on 13.05.24.
//

#include "RuntimeApplication.h"

namespace SierraEngine
{

    /* --- CONSTRUCTORS --- */

    RuntimeApplication::RuntimeApplication(const ApplicationCreateInfo& createInfo)
        : Application(createInfo)
    {

    }

    /* --- POLLING METHODS --- */

    bool RuntimeApplication::Update()
    {
        return true;
    }

}
