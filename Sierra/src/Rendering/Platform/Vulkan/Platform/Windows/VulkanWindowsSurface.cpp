//
// Created by Nikolay Kanchevski on 12.15.2023.
//

#include "VulkanWindowsSurface.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VkSurfaceKHR VulkanWindowsSurface::Create(const VulkanInstance &instance, const std::unique_ptr<Window> &window)
    {
        SR_ERROR_IF(window->GetAPI() != PlatformAPI::Win32, "[Vulkan]: Cannot create a Windows surface for Vulkan using window [{0}], which has a platform API, that differs from from [PlatformAPI::Win32]!", window->GetTitle());
        const Win32Window &win32Window = static_cast<const Win32Window&>(*window);

        // Set up surface create info
        VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = { };
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        surfaceCreateInfo.hinstance = win32Window.GetHInstance();
        surfaceCreateInfo.hwnd = win32Window.GetHwnd();

        // Create surface
        VkSurfaceKHR surface;
        const VkResult result = instance.GetFunctionTable().vkCreateWin32SurfaceKHR(instance.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create Linux surface for window [{0}]! Error code: {1}.", window->GetTitle(), result);

        return surface;
    }

}