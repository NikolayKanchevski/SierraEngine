//
// Created by Nikolay Kanchevski on 6.12.23.
//

#include "Swapchain.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    Swapchain::Swapchain(const SwapchainCreateInfo& createInfo)
    {
        SR_THROW_IF(createInfo.name.empty(), InvalidValueError("Cannot create swapchain, as specified name must not be empty"));
    }

}