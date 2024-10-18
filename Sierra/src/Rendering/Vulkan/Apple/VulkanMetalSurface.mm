//
// Created by Nikolay Kanchevski on 10.12.23.
//

#include "VulkanMetalSurface.h"

#if SR_PLATFORM_macOS
    #include "../../../Windowing/macOS/CocoaWindow.h"
#elif SR_PLATFORM_iOS
    #include "../../../Windowing/iOS/UIKitWindow.h"
#endif
#include "../VulkanErrorHandler.h"

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VkSurfaceKHR CreateVulkanMetalSurface(const VulkanContext& context, const Window& window)
    {
        VkMetalSurfaceCreateInfoEXT surfaceCreateInfo = { .sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT };
        #if SR_PLATFORM_macOS
            SR_THROW_IF(window.GetBackendType() != WindowingBackendType::Cocoa, UnexpectedTypeError(SR_FORMAT("Cannot create Vulkan Metal surface for window [{0}], as its platform backend differs from [PlatformBackendType::Cocoa]", window.GetTitle())));
            const CocoaWindow& cocoaWindow = static_cast<const CocoaWindow&>(window);
            surfaceCreateInfo.pLayer = reinterpret_cast<CAMetalLayer*>(cocoaWindow.GetNSView().layer);
        #elif SR_PLATFORM_iOS
            SR_THROW_IF(window.GetBackendType() != WindowingBackendType::UIKit, UnexpectedTypeError(SR_FORMAT("Cannot create Metal Vulkan surface for window [{0}], as its platform backend differs from [PlatformBackendType::UIKit]", window.GetTitle())));
            const UIKitWindow& uiKitWindow = static_cast<const UIKitWindow&>(window);
            surfaceCreateInfo.pLayer = reinterpret_cast<CAMetalLayer*>(uiKitWindow.GetUIView().layer);
        #endif

        // Create surface
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        const VkResult result = context.GetFunctionTable().vkCreateMetalSurfaceEXT(context.GetVulkanInstance(), &surfaceCreateInfo, nullptr, &surface);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create Vulkan Metal surface for window [{0}]", window.GetTitle()));

        return surface;
    }

}