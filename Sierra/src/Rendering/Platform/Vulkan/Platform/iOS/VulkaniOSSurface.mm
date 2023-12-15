//
// Created by Nikolay Kanchevski on 12.12.23.
//

#include "VulkaniOSSurface.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VkSurfaceKHR VulkaniOSSurface::Create(const VulkanInstance &instance, const std::unique_ptr<Window> &window)
    {
        SR_ERROR_IF(window->GetAPI() != PlatformAPI::UIKit, "[Vulkan]: Cannot create a iOS surface for Vulkan using window [{0}], which has a platform API, that differs from from [PlatformAPI::UIKit]!", window->GetTitle());
        const UIKitWindow &uiKitWindow = static_cast<UIKitWindow&>(*window);

        // Set up surface create info
        VkIOSSurfaceCreateInfoMVK surfaceCreateInfo = { };
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        surfaceCreateInfo.pView = (__bridge const void*) uiKitWindow.GetUIView();

        // Create surface
        VkSurfaceKHR surface;
        const VkResult result = instance.GetFunctionTable().vkCreateIOSSurfaceMVK(instance.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create iOS surface for window [{0}]! Error code: {1}.", window->GetTitle(), result);

        return surface;
    }

}
