//
// Created by Nikolay Kanchevski on 12.15.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the VulkanWin32Surface.h file is only allowed in Windows builds!"
#endif

#include "../VulkanInstance.h"
#include "../../../Windowing/Window.h"

namespace Sierra
{

    [[nodiscard]] SIERRA_API VkSurfaceKHR CreateVulkanWin32Surface(const VulkanInstance& instance, const Window& window);

}
