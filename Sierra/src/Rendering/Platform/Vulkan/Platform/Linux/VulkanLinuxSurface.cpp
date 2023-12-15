//
// Created by Nikolay Kanchevski on 12.15.23.
//

#include "VulkanLinuxSurface.h"

namespace Sierra
{

    VkSurfaceKHR VulkanLinuxSurface::Create(const VulkanInstance &instance, const std::unique_ptr<Window> &window)
    {
        SR_ERROR_IF(window->GetAPI() != PlatformAPI::X11, "[Vulkan]: Cannot create a Linux surface for Vulkan using window [{0}], which has a platform API, that differs from from [PlatformAPI::X11]!", window->GetTitle());
        const X11Window &x11Window = static_cast<X11Window&>(*window);

        // Set up surface create info
        VkXlibSurfaceCreateInfoKHR surfaceCreateInfo = { };
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.dpy = x11Window.GetDisplay();
        surfaceCreateInfo.window = x11Window.GetX11Window();

        // Create surface
        VkSurfaceKHR surface;
        const VkResult result = instance.GetFunctionTable().vkCreateXlibSurfaceKHR(instance.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create Linux surface for window [{0}]! Error code: {1}.", window->GetTitle(), result);

        return surface;
    }

}