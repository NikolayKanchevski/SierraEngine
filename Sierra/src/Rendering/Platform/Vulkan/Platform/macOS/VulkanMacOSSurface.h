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

    class SIERRA_API VulkanMacOSSurface final
    {
    private:
        static VkSurfaceKHR Create(const VulkanInstance &instance,  const Window &window);
        friend class VulkanSwapchain;

    };

}
