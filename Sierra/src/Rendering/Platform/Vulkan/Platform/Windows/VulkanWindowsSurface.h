//
// Created by Nikolay Kanchevski on 12.15.2023.
//

#pragma once

#if !SR_PLATFORM_WINDOWS
    #error "Including the VulkanWindowsSurface.h file is only allowed in Windows builds!"
#endif

#include "../../VulkanResource.h"
#include "../../VulkanInstance.h"
#include "../../../../../Core/Platform/Windows/Win32Window.h"

namespace Sierra
{

    namespace VulkanWindowsSurface
    {
        VkSurfaceKHR SIERRA_API Create(const VulkanInstance &instance, const Window &window);
    };

}
