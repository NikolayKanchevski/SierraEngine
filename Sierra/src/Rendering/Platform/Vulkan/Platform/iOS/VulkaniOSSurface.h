//
// Created by Nikolay Kanchevski on 12.12.23.
//

#pragma once

#if !SR_PLATFORM_iOS
    #error "Including the VulkaniOSSurface.h file is only allowed in iOS builds!"
#endif

#include "../../VulkanResource.h"
#include "../../VulkanInstance.h"
#include "../../../../../Core/Platform/iOS/UIKitWindow.h"

namespace Sierra
{

    class SIERRA_API VulkaniOSSurface final
    {
    private:
        static VkSurfaceKHR Create(const VulkanInstance &instance,  const Window &window);
        friend class VulkanSwapchain;

    };

}
