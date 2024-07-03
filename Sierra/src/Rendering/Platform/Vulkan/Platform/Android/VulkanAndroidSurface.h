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

    namespace VulkanAndroidSurface
    {
        VkSurfaceKHR SIERRA_API Create(const VulkanInstance &instance, const Window &window);
    };

}
