//
// Created by Nikolay Kanchevski on 12.15.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the VulkanWin32Surface.h file is only allowed in Windows builds!"
#endif

#include "../VulkanContext.h"
#include "../../../Windowing/Window.h"

namespace Sierra
{

    [[nodiscard]] VkSurfaceKHR SIERRA_API CreateVulkanWin32Surface(const VulkanContext& context, const Window& window);

}
