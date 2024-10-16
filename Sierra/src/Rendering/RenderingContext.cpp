//
// Created by Nikolay Kanchevski on 3.10.24.
//

#include "RenderingContext.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    RenderingContext::RenderingContext(const RenderingContextCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create rendering context, as specified name must not be empty"));
        SR_THROW_IF(createInfo.applicationName.empty(), InvalidValueError("Cannot create rendering context, as specified application name must not be empty"));
    }

}