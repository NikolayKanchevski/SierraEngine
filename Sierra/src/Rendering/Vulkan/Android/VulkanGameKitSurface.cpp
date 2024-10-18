//
// Created by Nikolay Kanchevski on 12/15/23.
//

#include "VulkanGameKitSurface.h"

#include "../../../Windowing/Android/GameKitWindow.h"

namespace Sierra
{

    VkSurfaceKHR CreateVulkanGameKitSurface(const VulkanContext& context, const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::GameKit, UnexpectedTypeError(SR_FORMAT("Cannot create Vulkan GameKit surface for window [{0}], as its platform backend differs from [PlatformBackendType::GameKit]", window.GetTitle())));
        const GameKitWindow& gameKitWindow = static_cast<const GameKitWindow&>(window);

        // Set up surface create info
        const VkAndroidSurfaceCreateInfoKHR surfaceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
            .window = gameKitWindow.GetNativeWindow()
        };

        // Create surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        const VkResult result = context.GetFunctionTable().vkCreateAndroidSurfaceKHR(context.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create Android surface for window [{0}]! Error code: {1}.", window.GetTitle(), static_cast<int32>(result));

        return surface;
    }

}