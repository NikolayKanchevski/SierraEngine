//
// Created by Nikolay Kanchevski on 12.15.2023.
//

#include "VulkanWin32Surface.h"

#include "../../../Windowing/Windows/Win32Window.h"
#include "../VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VkSurfaceKHR CreateVulkanWin32Surface(const VulkanContext& context, const Window& window)
    {
        SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Win32, UnexpectedTypeError(SR_FORMAT("Cannot create Vulkan Win32 surface for window [{0}], as its platform backend differs from [PlatformBackendType::Win32]", window.GetTitle())));
        const Win32Window& win32Window = static_cast<const Win32Window&>(window);

        // Set up surface create info
        const VkWin32SurfaceCreateInfoKHR surfaceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
            .hinstance = win32Window.GetHInstance(),
            .hwnd = win32Window.GetHwnd()
        };

        // Create surface
        VkSurfaceKHR surface;
        const VkResult result = context.GetFunctionTable().vkCreateWin32SurfaceKHR(context.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create Vulkan Win32 surface for window [{0}]", window.GetTitle()));

        return surface;
    }

}