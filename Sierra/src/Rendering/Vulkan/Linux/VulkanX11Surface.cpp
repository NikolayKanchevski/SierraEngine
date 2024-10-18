//
// Created by Nikolay Kanchevski on 12.15.23.
//

#include "VulkanX11Surface.h"

#include "../../../Windowing/Linux/X11Window.h"
#include "../VulkanErrorHandler.h"

namespace Sierra
{

    VkSurfaceKHR CreateVulkanX11Surface(const VulkanContext& context, const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::X11, UnexpectedTypeError(SR_FORMAT("Cannot create Vulkan X11 surface for window [{0}], as its platform backend differs from [PlatformBackendType::X11]", window.GetTitle())));
        const X11Window& x11Window = static_cast<const X11Window&>(window);

        // Set up surface create info
        const VkXlibSurfaceCreateInfoKHR surfaceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR,
            .dpy = x11Window.GetDisplay(),
            .window = x11Window.GetX11Window()
        };

        // Create surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        const VkResult result = context.GetFunctionTable().vkCreateXlibSurfaceKHR(context.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create Vulkan X11 surface for window [{0}]", window.GetTitle()));

        return surface;
    }

}