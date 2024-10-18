//
// Created by Nikolay Kanchevski on 12/15/23.
//

#pragma once

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the VulkanGameKitSurface.h file is only allowed in Android builds!"
#endif

#include "../VulkanContext.h"
#include "../../../Windowing/Window.h"

namespace Sierra
{

    [[nodiscard]] VkSurfaceKHR SIERRA_API CreateVulkanGameKitSurface(const VulkanContext& context, const Window& window);

}
