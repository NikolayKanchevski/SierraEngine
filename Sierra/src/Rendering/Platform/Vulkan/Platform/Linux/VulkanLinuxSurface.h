//
// Created by Nikolay Kanchevski on 12.15.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the VulkanLinuxSurface.h file is only allowed in Linux builds!"
#endif

#include "../../VulkanResource.h"
#include "../../VulkanInstance.h"
#include "../../../../../Core/Platform/Linux/X11Window.h"

namespace Sierra
{
    namespace VulkanLinuxSurface
    {
        VkSurfaceKHR SIERRA_API Create(const VulkanInstance& instance, const Window& window);
    };

}
