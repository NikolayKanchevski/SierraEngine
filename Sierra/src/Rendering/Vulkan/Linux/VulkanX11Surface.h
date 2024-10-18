//
// Created by Nikolay Kanchevski on 12.15.23.
//

#pragma once

#if !SR_PLATFORM_LINUX
    #error "Including the VulkanX11Surface.h file is only allowed in Linux builds!"
#endif

#include "../VulkanContext.h"
#include "../../../Windowing/Window.h"

namespace Sierra
{

    [[nodiscard]] SIERRA_API VkSurfaceKHR CreateVulkanX11Surface(const VulkanContext& context, const Window& window);

}
