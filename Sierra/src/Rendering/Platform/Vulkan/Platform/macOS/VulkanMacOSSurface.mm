//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanMacOSSurface.h"

#include <QuartzCore/QuartzCore.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VkSurfaceKHR VulkanMacOSSurface::Create(const VulkanInstance &instance, const std::unique_ptr<Window> &window)
    {
        SR_ERROR_IF(window->GetAPI() != PlatformAPI::Cocoa, "[Vulkan]: Cannot create a iOS surface for Vulkan using window [{0}], which has a platform API, that differs from from [PlatformAPI::Cocoa]!", window->GetTitle());
        const CocoaWindow &cocoaWindow = static_cast<CocoaWindow&>(*window);

        #if MAC_OS_X_VERSION_MAX_ALLOWED >= 101100
            // Set up surface create info
            VkMetalSurfaceCreateInfoEXT surfaceCreateInfo = { };
            surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
            surfaceCreateInfo.pLayer = reinterpret_cast<CAMetalLayer*>(cocoaWindow.GetNSView().layer);

            // Create surface
            VkSurfaceKHR surface;
            const VkResult result = instance.GetFunctionTable().vkCreateMetalSurfaceEXT(instance.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
            SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create macOS surface for window [{0}]! Error code: {1}.", window->GetTitle(), result);

            return surface;
        #else
            SR_ERROR("[Vulkan]: Could not create macOS surface for window [{0}], as macOS is running an outdated version with no support for Vulkan surfaces!", window.GetTitle());
        #endif

        return VK_NULL_HANDLE;
    }

}