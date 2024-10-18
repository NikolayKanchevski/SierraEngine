//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#if !SR_PLATFORM_APPLE
    #error "Including the VulkanMetalSurface.h file is only allowed in Apple builds!"
#endif

#include "../VulkanContext.h"
#include "../../../Windowing/Window.h"

namespace Sierra
{
    [[nodiscard]] SIERRA_API VkSurfaceKHR CreateVulkanMetalSurface(const VulkanContext& context, const Window& window);
}
