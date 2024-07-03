//
// Created by Nikolay Kanchevski on 10.12.23.
//

#pragma once

#if !SR_PLATFORM_macOS
    #error "Including the VulkanMacOSSurface.h file is only allowed in macOS builds!"
#endif

#include "../../VulkanResource.h"
#include "../../VulkanInstance.h"
#include "../../../../../Core/Platform/macOS/CocoaWindow.h"

namespace Sierra
{

    namespace VulkanMacOSSurface
    {
        VkSurfaceKHR SIERRA_API Create(const VulkanInstance &instance, const Window &window);
    }

}
