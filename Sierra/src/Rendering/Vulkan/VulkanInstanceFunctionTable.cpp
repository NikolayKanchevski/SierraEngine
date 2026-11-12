//
// Created by Nikolay Kantchevski on 9/28/26.
//

#include <vulkan/vulkan_core.h>
#include "VulkanInstanceFunctionTable.h"

namespace Sierra
{

    VulkanInstanceFunctionTable LoadVulkanInstanceFunctionTable(VkInstance instance) noexcept
    {
        return
        {
            #if defined(VK_VERSION_1_0)
                .vkCreateDevice                                 = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance, "vkCreateDevice")),
                .vkDestroyInstance                              = reinterpret_cast<PFN_vkDestroyInstance>(vkGetInstanceProcAddr(instance, "vkDestroyInstance")),
                .vkEnumerateDeviceExtensionProperties           = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(vkGetInstanceProcAddr(instance, "vkEnumerateDeviceExtensionProperties")),
                .vkEnumerateDeviceLayerProperties               = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(vkGetInstanceProcAddr(instance, "vkEnumerateDeviceLayerProperties")),
                .vkEnumeratePhysicalDevices                     = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices")),
                .vkGetDeviceProcAddr                            = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr")),
                .vkGetPhysicalDeviceFeatures                    = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures")),
                .vkGetPhysicalDeviceFormatProperties            = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties")),
                .vkGetPhysicalDeviceImageFormatProperties       = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties")),
                .vkGetPhysicalDeviceMemoryProperties            = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties")),
                .vkGetPhysicalDeviceProperties                  = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties")),
                .vkGetPhysicalDeviceQueueFamilyProperties       = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties")),
                .vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties")),
            #endif
            #if defined(VK_VERSION_1_1)
                .vkEnumeratePhysicalDeviceGroups                 = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroups>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroups")),
                .vkGetPhysicalDeviceExternalBufferProperties     = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferProperties")),
                .vkGetPhysicalDeviceExternalFenceProperties      = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFenceProperties")),
                .vkGetPhysicalDeviceExternalSemaphoreProperties  = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphoreProperties")),
                .vkGetPhysicalDeviceFeatures2                    = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2")),
                .vkGetPhysicalDeviceFormatProperties2            = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2")),
                .vkGetPhysicalDeviceImageFormatProperties2       = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2")),
                .vkGetPhysicalDeviceMemoryProperties2            = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2")),
                .vkGetPhysicalDeviceProperties2                  = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2")),
                .vkGetPhysicalDeviceQueueFamilyProperties2       = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2")),
                .vkGetPhysicalDeviceSparseImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2")),
            #endif
            #if defined(VK_VERSION_1_3)
                .vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceToolProperties")),
            #endif
            #if defined(VK_EXT_acquire_drm_display)
                .vkAcquireDrmDisplayEXT = reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(vkGetInstanceProcAddr(instance, "vkAcquireDrmDisplayEXT")),
                .vkGetDrmDisplayEXT     = reinterpret_cast<PFN_vkGetDrmDisplayEXT>(vkGetInstanceProcAddr(instance, "vkGetDrmDisplayEXT")),
            #endif
            #if defined(VK_EXT_acquire_xlib_display)
                .vkAcquireXlibDisplayEXT    = reinterpret_cast<PFN_vkAcquireXlibDisplayEXT>(vkGetInstanceProcAddr(instance, "vkAcquireXlibDisplayEXT")),
                        .vkGetRandROutputDisplayEXT = reinterpret_cast<PFN_vkGetRandROutputDisplayEXT>(vkGetInstanceProcAddr(instance, "vkGetRandROutputDisplayEXT")),
            #endif
            #if defined(VK_EXT_calibrated_timestamps)
                .vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT")),
            #endif
            #if defined(VK_EXT_debug_report)
                .vkCreateDebugReportCallbackEXT  = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT")),
                .vkDebugReportMessageEXT         = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT")),
                .vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT")),
            #endif
            #if defined(VK_EXT_debug_utils)
                .vkCmdBeginDebugUtilsLabelEXT    = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT")),
                .vkCmdEndDebugUtilsLabelEXT      = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT")),
                .vkCmdInsertDebugUtilsLabelEXT   = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT")),
                .vkCreateDebugUtilsMessengerEXT  = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")),
                .vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT")),
                .vkQueueBeginDebugUtilsLabelEXT  = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkQueueBeginDebugUtilsLabelEXT")),
                .vkQueueEndDebugUtilsLabelEXT    = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkQueueEndDebugUtilsLabelEXT")),
                .vkQueueInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkQueueInsertDebugUtilsLabelEXT")),
                .vkSetDebugUtilsObjectNameEXT    = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT")),
                .vkSetDebugUtilsObjectTagEXT     = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectTagEXT")),
                .vkSubmitDebugUtilsMessageEXT    = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT")),
            #endif
            #if defined(VK_EXT_direct_mode_display)
                .vkReleaseDisplayEXT = reinterpret_cast<PFN_vkReleaseDisplayEXT>(vkGetInstanceProcAddr(instance, "vkReleaseDisplayEXT")),
            #endif
            #if defined(VK_EXT_directfb_surface)
                .vkCreateDirectFBSurfaceEXT                        = reinterpret_cast<PFN_vkCreateDirectFBSurfaceEXT>(vkGetInstanceProcAddr(instance, "vkCreateDirectFBSurfaceEXT")),
                        .vkGetPhysicalDeviceDirectFBPresentationSupportEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT")),
            #endif
            #if defined(VK_EXT_display_surface_counter)
                .vkGetPhysicalDeviceSurfaceCapabilities2EXT = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2EXT")),
            #endif
            #if defined(VK_EXT_full_screen_exclusive)
                .vkGetPhysicalDeviceSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModes2EXT")),
            #endif
            #if defined(VK_EXT_headless_surface)
                .vkCreateHeadlessSurfaceEXT = reinterpret_cast<PFN_vkCreateHeadlessSurfaceEXT>(vkGetInstanceProcAddr(instance, "vkCreateHeadlessSurfaceEXT")),
            #endif
            #if defined(VK_EXT_metal_surface)
                .vkCreateMetalSurfaceEXT = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT")),
            #endif
            #if defined(VK_EXT_sample_locations)
                .vkGetPhysicalDeviceMultisamplePropertiesEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMultisamplePropertiesEXT")),
            #endif
            #if defined(VK_EXT_tooling_info)
                .vkGetPhysicalDeviceToolPropertiesEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceToolPropertiesEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceToolPropertiesEXT")),
            #endif
            #if defined(VK_FUCHSIA_imagepipe_surface)
                .vkCreateImagePipeSurfaceFUCHSIA = reinterpret_cast<PFN_vkCreateImagePipeSurfaceFUCHSIA>(vkGetInstanceProcAddr(instance, "vkCreateImagePipeSurfaceFUCHSIA")),
            #endif
            #if defined(VK_GGP_stream_descriptor_surface)
                .vkCreateStreamDescriptorSurfaceGGP = reinterpret_cast<PFN_vkCreateStreamDescriptorSurfaceGGP>(vkGetInstanceProcAddr(instance, "vkCreateStreamDescriptorSurfaceGGP")),
            #endif
            #if defined(VK_KHR_android_surface)
                .vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateAndroidSurfaceKHR")),
            #endif
            #if defined(VK_KHR_cooperative_matrix)
                .vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR")),
            #endif
            #if defined(VK_KHR_device_group_creation)
                .vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroupsKHR")),
            #endif
            #if defined(VK_KHR_display)
                .vkCreateDisplayModeKHR                       = reinterpret_cast<PFN_vkCreateDisplayModeKHR>(vkGetInstanceProcAddr(instance, "vkCreateDisplayModeKHR")),
                .vkCreateDisplayPlaneSurfaceKHR               = reinterpret_cast<PFN_vkCreateDisplayPlaneSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateDisplayPlaneSurfaceKHR")),
                .vkGetDisplayModePropertiesKHR                = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR")),
                .vkGetDisplayPlaneCapabilitiesKHR             = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR")),
                .vkGetDisplayPlaneSupportedDisplaysKHR        = reinterpret_cast<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneSupportedDisplaysKHR")),
                .vkGetPhysicalDeviceDisplayPlanePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR")),
                .vkGetPhysicalDeviceDisplayPropertiesKHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR")),
            #endif
            #if defined(VK_KHR_external_fence_capabilities)
                .vkGetPhysicalDeviceExternalFencePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR")),
            #endif
            #if defined(VK_KHR_external_memory_capabilities)
                .vkGetPhysicalDeviceExternalBufferPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR")),
            #endif
            #if defined(VK_KHR_external_semaphore_capabilities)
                .vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR")),
            #endif
            #if defined(VK_KHR_fragment_shading_rate)
                .vkGetPhysicalDeviceFragmentShadingRatesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFragmentShadingRatesKHR")),
            #endif
            #if defined(VK_KHR_get_display_properties2)
                .vkGetDisplayModeProperties2KHR                = reinterpret_cast<PFN_vkGetDisplayModeProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayModeProperties2KHR")),
                .vkGetDisplayPlaneCapabilities2KHR             = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilities2KHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilities2KHR")),
                .vkGetPhysicalDeviceDisplayPlaneProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR")),
                .vkGetPhysicalDeviceDisplayProperties2KHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayProperties2KHR")),
            #endif
            #if defined(VK_KHR_get_physical_device_properties2)
                .vkGetPhysicalDeviceFeatures2KHR                    = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2KHR")),
                .vkGetPhysicalDeviceFormatProperties2KHR            = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2KHR")),
                .vkGetPhysicalDeviceImageFormatProperties2KHR       = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2KHR")),
                .vkGetPhysicalDeviceMemoryProperties2KHR            = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2KHR")),
                .vkGetPhysicalDeviceProperties2KHR                  = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR")),
                .vkGetPhysicalDeviceQueueFamilyProperties2KHR       = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR")),
                .vkGetPhysicalDeviceSparseImageFormatProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR")),
            #endif
            #if defined(VK_KHR_get_surface_capabilities2)
                .vkGetPhysicalDeviceSurfaceCapabilities2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR")),
                .vkGetPhysicalDeviceSurfaceFormats2KHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR")),
            #endif
            #if defined(VK_KHR_performance_query)
                .vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR")),
                .vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR         = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR")),
            #endif
            #if defined(VK_KHR_surface)
                .vkDestroySurfaceKHR                       = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR")),
                .vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR")),
                .vkGetPhysicalDeviceSurfaceFormatsKHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR")),
                .vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR")),
                .vkGetPhysicalDeviceSurfaceSupportKHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR")),
            #endif
            #if defined(VK_KHR_video_encode_queue)
                .vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR")),
            #endif
            #if defined(VK_KHR_video_queue)
                .vkGetPhysicalDeviceVideoCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR")),
                .vkGetPhysicalDeviceVideoFormatPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoFormatPropertiesKHR")),
            #endif
            #if defined(VK_KHR_wayland_surface)
                .vkCreateWaylandSurfaceKHR                        = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR")),
                        .vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR")),
            #endif
            #if defined(VK_KHR_win32_surface)
                .vkCreateWin32SurfaceKHR                        = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR")),
                .vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR")),
            #endif
            #if defined(VK_KHR_xcb_surface)
                .vkCreateXcbSurfaceKHR                        = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateXcbSurfaceKHR")),
                .vkGetPhysicalDeviceXcbPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR")),
            #endif
            #if defined(VK_KHR_xlib_surface)
                .vkCreateXlibSurfaceKHR                        = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR")),
                .vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR")),
            #endif
            #if defined(VK_MVK_ios_surface)
                .vkCreateIOSSurfaceMVK = reinterpret_cast<PFN_vkCreateIOSSurfaceMVK>(vkGetInstanceProcAddr(instance, "vkCreateIOSSurfaceMVK")),
            #endif
            #if defined(VK_MVK_macos_surface)
                .vkCreateMacOSSurfaceMVK = reinterpret_cast<PFN_vkCreateMacOSSurfaceMVK>(vkGetInstanceProcAddr(instance, "vkCreateMacOSSurfaceMVK")),
            #endif
            #if defined(VK_NN_vi_surface)
                .vkCreateViSurfaceNN = reinterpret_cast<PFN_vkCreateViSurfaceNN>(vkGetInstanceProcAddr(instance, "vkCreateViSurfaceNN")),
            #endif
            #if defined(VK_NV_acquire_winrt_display)
                .vkAcquireWinrtDisplayNV = reinterpret_cast<PFN_vkAcquireWinrtDisplayNV>(vkGetInstanceProcAddr(instance, "vkAcquireWinrtDisplayNV")),
                .vkGetWinrtDisplayNV     = reinterpret_cast<PFN_vkGetWinrtDisplayNV>(vkGetInstanceProcAddr(instance, "vkGetWinrtDisplayNV")),
            #endif
            #if defined(VK_NV_cooperative_matrix)
                .vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = reinterpret_cast<PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV")),
            #endif
            #if defined(VK_NV_coverage_reduction_mode)
                .vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = reinterpret_cast<PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV")),
            #endif
            #if defined(VK_NV_external_memory_capabilities)
                .vkGetPhysicalDeviceExternalImageFormatPropertiesNV = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV")),
            #endif
            #if defined(VK_NV_optical_flow)
                .vkGetPhysicalDeviceOpticalFlowImageFormatsNV = reinterpret_cast<PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceOpticalFlowImageFormatsNV")),
            #endif
            #if defined(VK_QNX_screen_surface)
                .vkCreateScreenSurfaceQNX                        = reinterpret_cast<PFN_vkCreateScreenSurfaceQNX>(vkGetInstanceProcAddr(instance, "vkCreateScreenSurfaceQNX")),
                .vkGetPhysicalDeviceScreenPresentationSupportQNX = reinterpret_cast<PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceScreenPresentationSupportQNX")),
            #endif
            #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
                .vkGetPhysicalDevicePresentRectanglesKHR = reinterpret_cast<PFN_vkGetPhysicalDevicePresentRectanglesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDevicePresentRectanglesKHR")),
            #endif
        };
    }

}
