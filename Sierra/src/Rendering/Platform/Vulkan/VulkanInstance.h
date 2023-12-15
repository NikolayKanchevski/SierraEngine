//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include <vulkan/vulkan.h>
#include "VulkanResource.h"

namespace Sierra
{

    struct VulkanInstanceCreateInfo
    {
        const std::string &name = "Vulkan Instance";
    };

    class SIERRA_API VulkanAPIVersion
    {
    public:
        /* --- CONSTRUCTORS --- */
        inline VulkanAPIVersion(const uint32 version) : version(version) { }
        inline VulkanAPIVersion(const uint8 major, const uint8 minor, const uint8 patch) : version(VK_MAKE_API_VERSION(0, major, minor, patch)) { }

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline uint8 GetMajor() const { return VK_VERSION_MAJOR(version); }
        [[nodiscard]] inline uint8 GetMinor() const { return VK_VERSION_MINOR(version); }
        [[nodiscard]] inline uint8 GetPatch() const { return VK_VERSION_PATCH(version); }

        /* --- OPERATORS --- */
        inline operator uint32() const { return version; }

    private:
        const uint32 version;

    };

    class SIERRA_API VulkanInstance final : public VulkanResource
    {
    public:
        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkInstance GetVulkanInstance() const { return instance; }
        [[nodiscard]] inline auto& GetFunctionTable() const { return functionTable; }
        [[nodiscard]] bool IsExtensionLoaded(const std::string &extensionName) const;
        [[nodiscard]] VulkanAPIVersion GetAPIVersion() const;

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        friend class VulkanContext;
        explicit VulkanInstance(const VulkanInstanceCreateInfo &createInfo);

        VkInstance instance = VK_NULL_HANDLE;
        struct
        {
            #if defined(VK_VERSION_1_0)
                PFN_vkCreateDevice vkCreateDevice = nullptr;
                PFN_vkDestroyInstance vkDestroyInstance = nullptr;
                PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties = nullptr;
                PFN_vkEnumerateDeviceLayerProperties vkEnumerateDeviceLayerProperties = nullptr;
                PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices = nullptr;
                PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr = nullptr;
                PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures = nullptr;
                PFN_vkGetPhysicalDeviceFormatProperties vkGetPhysicalDeviceFormatProperties = nullptr;
                PFN_vkGetPhysicalDeviceImageFormatProperties vkGetPhysicalDeviceImageFormatProperties = nullptr;
                PFN_vkGetPhysicalDeviceMemoryProperties vkGetPhysicalDeviceMemoryProperties = nullptr;
                PFN_vkGetPhysicalDeviceProperties vkGetPhysicalDeviceProperties = nullptr;
                PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties = nullptr;
                PFN_vkGetPhysicalDeviceSparseImageFormatProperties vkGetPhysicalDeviceSparseImageFormatProperties = nullptr;
            #endif
            #if defined(VK_VERSION_1_1)
                PFN_vkEnumeratePhysicalDeviceGroups vkEnumeratePhysicalDeviceGroups = nullptr;
                PFN_vkGetPhysicalDeviceExternalBufferProperties vkGetPhysicalDeviceExternalBufferProperties = nullptr;
                PFN_vkGetPhysicalDeviceExternalFenceProperties vkGetPhysicalDeviceExternalFenceProperties = nullptr;
                PFN_vkGetPhysicalDeviceExternalSemaphoreProperties vkGetPhysicalDeviceExternalSemaphoreProperties = nullptr;
                PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2 = nullptr;
                PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2 = nullptr;
                PFN_vkGetPhysicalDeviceImageFormatProperties2 vkGetPhysicalDeviceImageFormatProperties2 = nullptr;
                PFN_vkGetPhysicalDeviceMemoryProperties2 vkGetPhysicalDeviceMemoryProperties2 = nullptr;
                PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2 = nullptr;
                PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2 = nullptr;
                PFN_vkGetPhysicalDeviceSparseImageFormatProperties2 vkGetPhysicalDeviceSparseImageFormatProperties2 = nullptr;
            #endif
            #if defined(VK_VERSION_1_3)
                PFN_vkGetPhysicalDeviceToolProperties vkGetPhysicalDeviceToolProperties = nullptr;
            #endif
            #if defined(VK_EXT_acquire_drm_display)
                PFN_vkAcquireDrmDisplayEXT vkAcquireDrmDisplayEXT = nullptr;
                PFN_vkGetDrmDisplayEXT vkGetDrmDisplayEXT = nullptr;
            #endif
            #if defined(VK_EXT_acquire_xlib_display)
                PFN_vkAcquireXlibDisplayEXT vkAcquireXlibDisplayEXT = nullptr;
                PFN_vkGetRandROutputDisplayEXT vkGetRandROutputDisplayEXT = nullptr;
            #endif
            #if defined(VK_EXT_calibrated_timestamps)
                PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = nullptr;
            #endif
            #if defined(VK_EXT_debug_report)
                PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = nullptr;
                PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = nullptr;
                PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
            #endif
            #if defined(VK_EXT_debug_utils)
                PFN_vkCmdBeginDebugUtilsLabelEXT vkCmdBeginDebugUtilsLabelEXT = nullptr;
                PFN_vkCmdEndDebugUtilsLabelEXT vkCmdEndDebugUtilsLabelEXT = nullptr;
                PFN_vkCmdInsertDebugUtilsLabelEXT vkCmdInsertDebugUtilsLabelEXT = nullptr;
                PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT = nullptr;
                PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT = nullptr;
                PFN_vkQueueBeginDebugUtilsLabelEXT vkQueueBeginDebugUtilsLabelEXT = nullptr;
                PFN_vkQueueEndDebugUtilsLabelEXT vkQueueEndDebugUtilsLabelEXT = nullptr;
                PFN_vkQueueInsertDebugUtilsLabelEXT vkQueueInsertDebugUtilsLabelEXT = nullptr;
                PFN_vkSetDebugUtilsObjectNameEXT vkSetDebugUtilsObjectNameEXT = nullptr;
                PFN_vkSetDebugUtilsObjectTagEXT vkSetDebugUtilsObjectTagEXT = nullptr;
                PFN_vkSubmitDebugUtilsMessageEXT vkSubmitDebugUtilsMessageEXT = nullptr;
            #endif
            #if defined(VK_EXT_direct_mode_display)
                PFN_vkReleaseDisplayEXT vkReleaseDisplayEXT = nullptr;
            #endif
            #if defined(VK_EXT_directfb_surface)
                PFN_vkCreateDirectFBSurfaceEXT vkCreateDirectFBSurfaceEXT = nullptr;
                PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT vkGetPhysicalDeviceDirectFBPresentationSupportEXT = nullptr;
            #endif
            #if defined(VK_EXT_display_surface_counter)
                PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT vkGetPhysicalDeviceSurfaceCapabilities2EXT = nullptr;
            #endif
            #if defined(VK_EXT_full_screen_exclusive)
                PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT = nullptr;
            #endif
            #if defined(VK_EXT_headless_surface)
                PFN_vkCreateHeadlessSurfaceEXT vkCreateHeadlessSurfaceEXT = nullptr;
            #endif
            #if defined(VK_EXT_metal_surface)
                PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT = nullptr;
            #endif
            #if defined(VK_EXT_sample_locations)
                PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT vkGetPhysicalDeviceMultisamplePropertiesEXT = nullptr;
            #endif
            #if defined(VK_EXT_tooling_info)
                PFN_vkGetPhysicalDeviceToolPropertiesEXT vkGetPhysicalDeviceToolPropertiesEXT = nullptr;
            #endif
            #if defined(VK_FUCHSIA_imagepipe_surface)
                PFN_vkCreateImagePipeSurfaceFUCHSIA vkCreateImagePipeSurfaceFUCHSIA = nullptr;
            #endif
            #if defined(VK_GGP_stream_descriptor_surface)
                PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP = nullptr;
            #endif
            #if defined(VK_KHR_android_surface)
                PFN_vkCreateAndroidSurfaceKHR vkCreateAndroidSurfaceKHR = nullptr;
            #endif
            #if defined(VK_KHR_cooperative_matrix)
                PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR = nullptr;
            #endif
            #if defined(VK_KHR_device_group_creation)
                PFN_vkEnumeratePhysicalDeviceGroupsKHR vkEnumeratePhysicalDeviceGroupsKHR = nullptr;
            #endif
            #if defined(VK_KHR_display)
                PFN_vkCreateDisplayModeKHR vkCreateDisplayModeKHR = nullptr;
                PFN_vkCreateDisplayPlaneSurfaceKHR vkCreateDisplayPlaneSurfaceKHR = nullptr;
                PFN_vkGetDisplayModePropertiesKHR vkGetDisplayModePropertiesKHR = nullptr;
                PFN_vkGetDisplayPlaneCapabilitiesKHR vkGetDisplayPlaneCapabilitiesKHR = nullptr;
                PFN_vkGetDisplayPlaneSupportedDisplaysKHR vkGetDisplayPlaneSupportedDisplaysKHR = nullptr;
                PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR vkGetPhysicalDeviceDisplayPlanePropertiesKHR = nullptr;
                PFN_vkGetPhysicalDeviceDisplayPropertiesKHR vkGetPhysicalDeviceDisplayPropertiesKHR = nullptr;
            #endif
            #if defined(VK_KHR_external_fence_capabilities)
                PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR vkGetPhysicalDeviceExternalFencePropertiesKHR = nullptr;
            #endif
            #if defined(VK_KHR_external_memory_capabilities)
                PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR vkGetPhysicalDeviceExternalBufferPropertiesKHR = nullptr;
            #endif
            #if defined(VK_KHR_external_semaphore_capabilities)
                PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = nullptr;
            #endif
            #if defined(VK_KHR_fragment_shading_rate)
                PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR vkGetPhysicalDeviceFragmentShadingRatesKHR = nullptr;
            #endif
            #if defined(VK_KHR_get_display_properties2)
                PFN_vkGetDisplayModeProperties2KHR vkGetDisplayModeProperties2KHR = nullptr;
                PFN_vkGetDisplayPlaneCapabilities2KHR vkGetDisplayPlaneCapabilities2KHR = nullptr;
                PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR vkGetPhysicalDeviceDisplayPlaneProperties2KHR = nullptr;
                PFN_vkGetPhysicalDeviceDisplayProperties2KHR vkGetPhysicalDeviceDisplayProperties2KHR = nullptr;
            #endif
            #if defined(VK_KHR_get_physical_device_properties2)
                PFN_vkGetPhysicalDeviceFeatures2KHR vkGetPhysicalDeviceFeatures2KHR = nullptr;
                PFN_vkGetPhysicalDeviceFormatProperties2KHR vkGetPhysicalDeviceFormatProperties2KHR = nullptr;
                PFN_vkGetPhysicalDeviceImageFormatProperties2KHR vkGetPhysicalDeviceImageFormatProperties2KHR = nullptr;
                PFN_vkGetPhysicalDeviceMemoryProperties2KHR vkGetPhysicalDeviceMemoryProperties2KHR = nullptr;
                PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR = nullptr;
                PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR vkGetPhysicalDeviceQueueFamilyProperties2KHR = nullptr;
                PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR vkGetPhysicalDeviceSparseImageFormatProperties2KHR = nullptr;
            #endif
            #if defined(VK_KHR_get_surface_capabilities2)
                PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR = nullptr;
                PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR = nullptr;
            #endif
            #if defined(VK_KHR_performance_query)
                PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = nullptr;
                PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR = nullptr;
            #endif
            #if defined(VK_KHR_surface)
                PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR = nullptr;
                PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR = nullptr;
                PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR = nullptr;
                PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR = nullptr;
                PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR = nullptr;
            #endif
            #if defined(VK_KHR_video_encode_queue)
                PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = nullptr;
            #endif
            #if defined(VK_KHR_video_queue)
                PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR vkGetPhysicalDeviceVideoCapabilitiesKHR = nullptr;
                PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR vkGetPhysicalDeviceVideoFormatPropertiesKHR = nullptr;
            #endif
            #if defined(VK_KHR_wayland_surface)
                PFN_vkCreateWaylandSurfaceKHR vkCreateWaylandSurfaceKHR = nullptr;
                PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR vkGetPhysicalDeviceWaylandPresentationSupportKHR = nullptr;
            #endif
            #if defined(VK_KHR_win32_surface)
                PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR = nullptr;
                PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR = nullptr;
            #endif
            #if defined(VK_KHR_xcb_surface)
                PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR = nullptr;
                PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR vkGetPhysicalDeviceXcbPresentationSupportKHR = nullptr;
            #endif
            #if defined(VK_KHR_xlib_surface)
                PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR = nullptr;
                PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR vkGetPhysicalDeviceXlibPresentationSupportKHR = nullptr;
            #endif
            #if defined(VK_MVK_ios_surface)
                PFN_vkCreateIOSSurfaceMVK vkCreateIOSSurfaceMVK = nullptr;
            #endif
            #if defined(VK_MVK_macos_surface)
                PFN_vkCreateMacOSSurfaceMVK vkCreateMacOSSurfaceMVK = nullptr;
            #endif
            #if defined(VK_NN_vi_surface)
                PFN_vkCreateViSurfaceNN vkCreateViSurfaceNN = nullptr;
            #endif
            #if defined(VK_NV_acquire_winrt_display)
                PFN_vkAcquireWinrtDisplayNV vkAcquireWinrtDisplayNV = nullptr;
                PFN_vkGetWinrtDisplayNV vkGetWinrtDisplayNV = nullptr;
            #endif
            #if defined(VK_NV_cooperative_matrix)
                PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = nullptr;
            #endif
            #if defined(VK_NV_coverage_reduction_mode)
                PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = nullptr;
            #endif
            #if defined(VK_NV_external_memory_capabilities)
                PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV vkGetPhysicalDeviceExternalImageFormatPropertiesNV = nullptr;
            #endif
            #if defined(VK_NV_optical_flow)
                PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV vkGetPhysicalDeviceOpticalFlowImageFormatsNV = nullptr;
            #endif
            #if defined(VK_QNX_screen_surface)
                PFN_vkCreateScreenSurfaceQNX vkCreateScreenSurfaceQNX = nullptr;
                PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX vkGetPhysicalDeviceScreenPresentationSupportQNX = nullptr;
            #endif
            #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
                PFN_vkGetPhysicalDevicePresentRectanglesKHR vkGetPhysicalDevicePresentRectanglesKHR = nullptr;
            #endif
        } functionTable = { };

        struct InstanceExtension
        {
            std::string name;
            bool requiredOnlyIfSupported = false;
        };

        const std::vector<InstanceExtension> INSTANCE_EXTENSIONS_TO_QUERY
        {
            { .name = VK_KHR_SURFACE_EXTENSION_NAME },
            #if SR_PLATFORM_WINDOWS
                { .name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME },
            #elif SR_PLATFORM_LINUX
                { .name = VK_KHR_XLIB_SURFACE_EXTENSION_NAME },
            #elif SR_PLATFORM_macOS
                { .name = VK_MVK_MACOS_SURFACE_EXTENSION_NAME },
            #elif SR_PLATFORM_ANDROID
                { .name = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME },
            #elif SR_PLATFORM_iOS
                { .name = VK_MVK_IOS_SURFACE_EXTENSION_NAME },
            #endif
            #if SR_PLATFORM_APPLE
                {
                    .name = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                    .requiredOnlyIfSupported = true
                },
            #endif
            #if SR_ENABLE_LOGGING && !SR_PLATFORM_MOBILE
                {
                    .name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                    .requiredOnlyIfSupported = true
                },
            #endif
        };

        std::vector<Hash> loadedExtensions;
        bool AddExtensionIfSupported(const InstanceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions);

        #if SR_ENABLE_LOGGING && !SR_PLATFORM_MOBILE
            VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
            static bool ValidationLayersSupported(const std::vector<const char*> &layers);
        #endif

    };

}
