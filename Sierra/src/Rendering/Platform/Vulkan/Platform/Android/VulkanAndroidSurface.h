//
// Created by Nikolay Kanchevski on 12/15/23.
//

#pragma once

#if !SR_PLATFORM_ANDROID
    #error "Including the VulkanAndroidSurface.h file is only allowed in Android builds!"
#endif

#include "../../VulkanResource.h"
#include "../../VulkanInstance.h"
#include "../../../../../Core/Platform/Android/GameActivityWindow.h"

namespace Sierra
{

    class SIERRA_API VulkanAndroidSurface final
    {
    private:
        static VkSurfaceKHR Create(const VulkanInstance &instance,  const Window &window);
        friend class VulkanSwapchain;

    };

}
