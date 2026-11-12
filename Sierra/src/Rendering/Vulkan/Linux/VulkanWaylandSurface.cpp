//
// Created by Nikolay Kanchevski on 11.12.26.
//

#include "VulkanWaylandSurface.h"

#include "../../../Windowing/Linux/Wayland/WaylandWindow.h"
#include "../VulkanErrorHandler.h"

namespace Sierra
{

    VkSurfaceKHR CreateVulkanWaylandSurface(const VulkanInstance& instance, const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Wayland, UnexpectedTypeError(SR_FORMAT("Cannot create Vulkan Wayland surface for window [{0}], as its windowing backend differs from [WindowingBackendType::Wayland]", window.GetTitle())));
        SR_THROW_IF(!instance.IsExtensionLoaded(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME), UnsupportedFeatureError(SR_FORMAT("Cannot create Vulkan Wayland surface for window [{0}], as Vulkan instance extension [{1}] is unsupported", window.GetTitle(), VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME)));
        const WaylandWindow& waylandWindow = static_cast<const WaylandWindow&>(window);

        // Set up surface create info
        const VkWaylandSurfaceCreateInfoKHR surfaceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR,
            .display = waylandWindow.GetDisplay(),
            .surface = waylandWindow.GetSurface()
        };

        // Create surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        const VkResult result = instance.GetFunctionTable().vkCreateWaylandSurfaceKHR(instance.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create Vulkan Wayland surface for window [{0}]", window.GetTitle()));

        return surface;
    }

}
