//
// Created by Nikolay Kanchevski on 12/15/23.
//

#include "VulkanAndroidSurface.h"

namespace Sierra
{

    VkSurfaceKHR VulkanAndroidSurface::Create(const VulkanInstance &instance, const std::unique_ptr<Window> &window)
    {
        SR_ERROR_IF(window->GetAPI() != PlatformAPI::GameActivity, "[Vulkan]: Cannot create a Android surface for Vulkan using window [{0}], which has a platform API, that differs from from [PlatformAPI::GameActivity]!", window->GetTitle());
        const GameActivityWindow &gameActivityWindow = static_cast<const GameActivityWindow&>(*window);

        // Set up surface create info
        VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo = { };
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.window = gameActivityWindow.GetNativeWindow();

        // Create surface
        VkSurfaceKHR surface;
        const VkResult result = instance.GetFunctionTable().vkCreateAndroidSurfaceKHR(instance.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create Android surface for window [{0}]! Error code: {1}.", window->GetTitle(), result);

        return surface;
    }

}