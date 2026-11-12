//
// Created by Nikolay Kanchevski on 11.12.26.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the VulkanWaylandSurface.h file is only allowed in Linux builds!"
#endif

#include "../VulkanInstance.h"
#include "../../../Windowing/Window.h"

namespace Sierra
{

    [[nodiscard]] SIERRA_API VkSurfaceKHR CreateVulkanWaylandSurface(const VulkanInstance& instance, const Window& window);

}
