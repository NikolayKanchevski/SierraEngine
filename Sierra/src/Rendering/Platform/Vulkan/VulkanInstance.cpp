//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include <vulkan/vulkan.h>
#include "VulkanInstance.h"

#include <vulkan/vulkan_metal.h>

namespace Sierra
{

    namespace
    {
        struct Layer
        {
            std::string_view name;
        };

        #if SR_ENABLE_LOGGING
            constexpr std::array LAYERS_TO_QUERY
            {
                Layer {
                    .name = "VK_LAYER_KHRONOS_validation"
                }
            };
        #endif

        struct Extension
        {
            std::string_view name;
            bool requiredOnlyIfSupported = false;
        };

        constexpr std::array INSTANCE_EXTENSIONS_TO_QUERY
        {
            Extension {
                .name = VK_KHR_SURFACE_EXTENSION_NAME
            },
            Extension {
                .name = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
            },
            #if SR_ENABLE_LOGGING
                Extension {
                    .name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                    .requiredOnlyIfSupported = true
                },
            #endif
            #if SR_PLATFORM_WINDOWS
                Extension { .name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME },
            #elif SR_PLATFORM_LINUX
                Extension { .name = VK_KHR_XLIB_SURFACE_EXTENSION_NAME },
            #elif SR_PLATFORM_ANDROID
                Extension { .name = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME },
            #elif SR_PLATFORM_APPLE
                Extension {
                    .name = VK_EXT_METAL_SURFACE_EXTENSION_NAME
                },
                Extension {
                    .name = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                    .requiredOnlyIfSupported = true
                },
            #endif
        };
    }

    /* --- CONSTRUCTORS --- */

    VulkanInstance::VulkanInstance(const VulkanInstanceCreateInfo&)
    {
        #if SR_PLATFORM_APPLE
            // Optional MoltenVK features must be explicitly enabled prior to performing any API calls
            setenv("MVK_DEBUG", "1", true);
            setenv("MVK_CONFIG_USE_METAL_ARGUMENT_BUFFERS", "2", true);
        #endif

        // Set up application info
        const Version version = GetAPIVersion();
        const VkApplicationInfo applicationInfo
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = "Sierra Application",
            .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .pEngineName = "Sierra",
            .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
            .apiVersion = VK_MAKE_API_VERSION(0, version.GetMajor(), version.GetMinor(), version.GetPatch())
        };

        // Retrieve supported extension count
        uint32 supportedExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);

        // Retrieve supported extensions
        std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

        // Define extensions array
        std::array<const char*, INSTANCE_EXTENSIONS_TO_QUERY.size()> extensions = { };
        loadedExtensions.resize(INSTANCE_EXTENSIONS_TO_QUERY.size());

        // Load extensions
        uint32 i = 0;
        for (const Extension &requestedExtension : INSTANCE_EXTENSIONS_TO_QUERY)
        {
            bool extensionFound = false;
            for (const VkExtensionProperties &supportedExtension : supportedExtensions)
            {
                if (strcmp(requestedExtension.name.data(), supportedExtension.extensionName) == 0)
                {
                    extensions[i] = requestedExtension.name.data();
                    loadedExtensions[i] = std::hash<std::string_view>{}(requestedExtension.name);
                    i++;

                    extensionFound = true;
                    break;
                }
            }

            SR_WARNING_IF(!extensionFound && !requestedExtension.requiredOnlyIfSupported, "[Vulkan]: Device extension [{0}] requested but not supported! Extension will be discarded, but issues may occur if extensions' support is not checked before their usage!", requestedExtension.name);
        }

        // Set up instance create info
        VkInstanceCreateInfo instanceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            #if SR_PLATFORM_APPLE
                .flags = static_cast<uint32>(IsExtensionLoaded(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME)) * VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
            #endif
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = 0,
            .ppEnabledLayerNames = nullptr,
            .enabledExtensionCount = i,
            .ppEnabledExtensionNames = extensions.data()
        };

         #if SR_ENABLE_LOGGING
            // Retrieve supported layer count
            uint32 supportedLayerCount = 0;
            vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

            // Retrieve supported layers
            std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
            vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

            // Define debug data
            std::array<const char*, LAYERS_TO_QUERY.size()> layers = { };
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = { };

            bool validationEnabled = false;

            // Enable validation & layers
            const bool validationSupported = IsExtensionLoaded(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            while (validationSupported)
            {
                // Load layers
                i = 0;
                for (const Layer &requestedLayer : LAYERS_TO_QUERY)
                {
                    for (const VkLayerProperties &supportedLayer : supportedLayers)
                    {
                        if (strcmp(requestedLayer.name.data(), supportedLayer.layerName) == 0)
                        {
                            layers[i] = supportedLayer.layerName;
                            i++;

                            break;
                        }
                    }
                }

                // Break if no layers are found
                if (i == 0) break;

                // Set up debug messenger info
                debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debugMessengerCreateInfo.pfnUserCallback = [](const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*) -> VkBool32
                {
                    switch (messageSeverity)
                    {
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        {
                            SR_WARNING("[Vulkan]: Warning - {0}", callbackData->pMessage);
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        {
                            SR_ERROR("[Vulkan]: Error - {0}", callbackData->pMessage);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }

                    return VK_FALSE;
                };

                // Re-assign layer information in instance info
                instanceCreateInfo.enabledLayerCount = i;
                instanceCreateInfo.ppEnabledLayerNames = layers.data();
                instanceCreateInfo.pNext = &debugMessengerCreateInfo;

                validationEnabled = true;
                break;
            }
            SR_WARNING_IF(!validationEnabled, "[Vulkan]: Cannot enable Vulkan validation, as the required [{0}] extension in unsupported!", VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        #endif

        // Create instance
        const VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        SR_ERROR_IF(result != VK_SUCCESS, "[Vulkan]: Could not create instance! Error code: {0}.", static_cast<int32>(result));

        // Load function pointers
        #pragma region Function Pointers
            #if defined(VK_VERSION_1_0)
                functionTable.vkCreateDevice                                 = reinterpret_cast<PFN_vkCreateDevice>(vkGetInstanceProcAddr(instance, "vkCreateDevice"));
                functionTable.vkDestroyInstance                              = reinterpret_cast<PFN_vkDestroyInstance>(vkGetInstanceProcAddr(instance, "vkDestroyInstance"));
                functionTable.vkEnumerateDeviceExtensionProperties           = reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(vkGetInstanceProcAddr(instance, "vkEnumerateDeviceExtensionProperties"));
                functionTable.vkEnumerateDeviceLayerProperties               = reinterpret_cast<PFN_vkEnumerateDeviceLayerProperties>(vkGetInstanceProcAddr(instance, "vkEnumerateDeviceLayerProperties"));
                functionTable.vkEnumeratePhysicalDevices                     = reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDevices"));
                functionTable.vkGetDeviceProcAddr                            = reinterpret_cast<PFN_vkGetDeviceProcAddr>(vkGetInstanceProcAddr(instance, "vkGetDeviceProcAddr"));
                functionTable.vkGetPhysicalDeviceFeatures                    = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures"));
                functionTable.vkGetPhysicalDeviceFormatProperties            = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties"));
                functionTable.vkGetPhysicalDeviceImageFormatProperties       = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties"));
                functionTable.vkGetPhysicalDeviceMemoryProperties            = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties"));
                functionTable.vkGetPhysicalDeviceProperties                  = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties"));
                functionTable.vkGetPhysicalDeviceQueueFamilyProperties       = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties"));
                functionTable.vkGetPhysicalDeviceSparseImageFormatProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties"));
            #endif
            #if defined(VK_VERSION_1_1)
                functionTable.vkEnumeratePhysicalDeviceGroups                 = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroups>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroups"));
                functionTable.vkGetPhysicalDeviceExternalBufferProperties     = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferProperties"));
                functionTable.vkGetPhysicalDeviceExternalFenceProperties      = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFenceProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFenceProperties"));
                functionTable.vkGetPhysicalDeviceExternalSemaphoreProperties  = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphoreProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphoreProperties"));
                functionTable.vkGetPhysicalDeviceFeatures2                    = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2"));
                functionTable.vkGetPhysicalDeviceFormatProperties2            = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2"));
                functionTable.vkGetPhysicalDeviceImageFormatProperties2       = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2"));
                functionTable.vkGetPhysicalDeviceMemoryProperties2            = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2"));
                functionTable.vkGetPhysicalDeviceProperties2                  = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2"));
                functionTable.vkGetPhysicalDeviceQueueFamilyProperties2       = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2"));
                functionTable.vkGetPhysicalDeviceSparseImageFormatProperties2 = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2"));
            #endif
            #if defined(VK_VERSION_1_3)
                functionTable.vkGetPhysicalDeviceToolProperties = reinterpret_cast<PFN_vkGetPhysicalDeviceToolProperties>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceToolProperties"));
            #endif
            #if defined(VK_EXT_acquire_drm_display)
                functionTable.vkAcquireDrmDisplayEXT = reinterpret_cast<PFN_vkAcquireDrmDisplayEXT>(vkGetInstanceProcAddr(instance, "vkAcquireDrmDisplayEXT"));
                functionTable.vkGetDrmDisplayEXT     = reinterpret_cast<PFN_vkGetDrmDisplayEXT>(vkGetInstanceProcAddr(instance, "vkGetDrmDisplayEXT"));
            #endif
            #if defined(VK_EXT_acquire_xlib_display)
                functionTable.vkAcquireXlibDisplayEXT    = reinterpret_cast<PFN_vkAcquireXlibDisplayEXT>(vkGetInstanceProcAddr(instance, "vkAcquireXlibDisplayEXT"));
                functionTable.vkGetRandROutputDisplayEXT = reinterpret_cast<PFN_vkGetRandROutputDisplayEXT>(vkGetInstanceProcAddr(instance, "vkGetRandROutputDisplayEXT"));
            #endif
            #if defined(VK_EXT_calibrated_timestamps)
                functionTable.vkGetPhysicalDeviceCalibrateableTimeDomainsEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceCalibrateableTimeDomainsEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceCalibrateableTimeDomainsEXT"));
            #endif
            #if defined(VK_EXT_debug_report)
                functionTable.vkCreateDebugReportCallbackEXT  = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
                functionTable.vkDebugReportMessageEXT         = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));
                functionTable.vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
            #endif
            #if defined(VK_EXT_debug_utils)
                functionTable.vkCmdBeginDebugUtilsLabelEXT    = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT"));
                functionTable.vkCmdEndDebugUtilsLabelEXT      = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT"));
                functionTable.vkCmdInsertDebugUtilsLabelEXT   = reinterpret_cast<PFN_vkCmdInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkCmdInsertDebugUtilsLabelEXT"));
                functionTable.vkCreateDebugUtilsMessengerEXT  = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
                functionTable.vkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
                functionTable.vkQueueBeginDebugUtilsLabelEXT  = reinterpret_cast<PFN_vkQueueBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkQueueBeginDebugUtilsLabelEXT"));
                functionTable.vkQueueEndDebugUtilsLabelEXT    = reinterpret_cast<PFN_vkQueueEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkQueueEndDebugUtilsLabelEXT"));
                functionTable.vkQueueInsertDebugUtilsLabelEXT = reinterpret_cast<PFN_vkQueueInsertDebugUtilsLabelEXT>(vkGetInstanceProcAddr(instance, "vkQueueInsertDebugUtilsLabelEXT"));
                functionTable.vkSetDebugUtilsObjectNameEXT    = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT"));
                functionTable.vkSetDebugUtilsObjectTagEXT     = reinterpret_cast<PFN_vkSetDebugUtilsObjectTagEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectTagEXT"));
                functionTable.vkSubmitDebugUtilsMessageEXT    = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT"));
            #endif
            #if defined(VK_EXT_direct_mode_display)
                functionTable.vkReleaseDisplayEXT = reinterpret_cast<PFN_vkReleaseDisplayEXT>(vkGetInstanceProcAddr(instance, "vkReleaseDisplayEXT"));
            #endif
            #if defined(VK_EXT_directfb_surface)
                functionTable.vkCreateDirectFBSurfaceEXT                        = reinterpret_cast<PFN_vkCreateDirectFBSurfaceEXT>(vkGetInstanceProcAddr(instance, "vkCreateDirectFBSurfaceEXT"));
                functionTable.vkGetPhysicalDeviceDirectFBPresentationSupportEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceDirectFBPresentationSupportEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDirectFBPresentationSupportEXT"));
            #endif
            #if defined(VK_EXT_display_surface_counter)
                functionTable.vkGetPhysicalDeviceSurfaceCapabilities2EXT = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2EXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2EXT"));
            #endif
            #if defined(VK_EXT_full_screen_exclusive)
                functionTable.vkGetPhysicalDeviceSurfacePresentModes2EXT = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModes2EXT"));
            #endif
            #if defined(VK_EXT_headless_surface)
                functionTable.vkCreateHeadlessSurfaceEXT = reinterpret_cast<PFN_vkCreateHeadlessSurfaceEXT>(vkGetInstanceProcAddr(instance, "vkCreateHeadlessSurfaceEXT"));
            #endif
            #if defined(VK_EXT_metal_surface)
                functionTable.vkCreateMetalSurfaceEXT = reinterpret_cast<PFN_vkCreateMetalSurfaceEXT>(vkGetInstanceProcAddr(instance, "vkCreateMetalSurfaceEXT"));
            #endif
            #if defined(VK_EXT_sample_locations)
                functionTable.vkGetPhysicalDeviceMultisamplePropertiesEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceMultisamplePropertiesEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMultisamplePropertiesEXT"));
            #endif
            #if defined(VK_EXT_tooling_info)
                functionTable.vkGetPhysicalDeviceToolPropertiesEXT = reinterpret_cast<PFN_vkGetPhysicalDeviceToolPropertiesEXT>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceToolPropertiesEXT"));
            #endif
            #if defined(VK_FUCHSIA_imagepipe_surface)
                functionTable.vkCreateImagePipeSurfaceFUCHSIA = reinterpret_cast<PFN_vkCreateImagePipeSurfaceFUCHSIA>(vkGetInstanceProcAddr(instance, "vkCreateImagePipeSurfaceFUCHSIA"));
            #endif
            #if defined(VK_GGP_stream_descriptor_surface)
                functionTable.vkCreateStreamDescriptorSurfaceGGP = reinterpret_cast<PFN_vkCreateStreamDescriptorSurfaceGGP>(vkGetInstanceProcAddr(instance, "vkCreateStreamDescriptorSurfaceGGP"));
            #endif
            #if defined(VK_KHR_android_surface)
                functionTable.vkCreateAndroidSurfaceKHR = reinterpret_cast<PFN_vkCreateAndroidSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateAndroidSurfaceKHR"));
            #endif
            #if defined(VK_KHR_cooperative_matrix)
                functionTable.vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceCooperativeMatrixPropertiesKHR"));
            #endif
            #if defined(VK_KHR_device_group_creation)
                functionTable.vkEnumeratePhysicalDeviceGroupsKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceGroupsKHR>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceGroupsKHR"));
            #endif
            #if defined(VK_KHR_display)
                functionTable.vkCreateDisplayModeKHR                       = reinterpret_cast<PFN_vkCreateDisplayModeKHR>(vkGetInstanceProcAddr(instance, "vkCreateDisplayModeKHR"));
                functionTable.vkCreateDisplayPlaneSurfaceKHR               = reinterpret_cast<PFN_vkCreateDisplayPlaneSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateDisplayPlaneSurfaceKHR"));
                functionTable.vkGetDisplayModePropertiesKHR                = reinterpret_cast<PFN_vkGetDisplayModePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayModePropertiesKHR"));
                functionTable.vkGetDisplayPlaneCapabilitiesKHR             = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilitiesKHR"));
                functionTable.vkGetDisplayPlaneSupportedDisplaysKHR        = reinterpret_cast<PFN_vkGetDisplayPlaneSupportedDisplaysKHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneSupportedDisplaysKHR"));
                functionTable.vkGetPhysicalDeviceDisplayPlanePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlanePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlanePropertiesKHR"));
                functionTable.vkGetPhysicalDeviceDisplayPropertiesKHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPropertiesKHR"));
            #endif
            #if defined(VK_KHR_external_fence_capabilities)
                functionTable.vkGetPhysicalDeviceExternalFencePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalFencePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalFencePropertiesKHR"));
            #endif
            #if defined(VK_KHR_external_memory_capabilities)
                functionTable.vkGetPhysicalDeviceExternalBufferPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalBufferPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalBufferPropertiesKHR"));
            #endif
            #if defined(VK_KHR_external_semaphore_capabilities)
                functionTable.vkGetPhysicalDeviceExternalSemaphorePropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalSemaphorePropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalSemaphorePropertiesKHR"));
            #endif
            #if defined(VK_KHR_fragment_shading_rate)
                functionTable.vkGetPhysicalDeviceFragmentShadingRatesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceFragmentShadingRatesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFragmentShadingRatesKHR"));
            #endif
            #if defined(VK_KHR_get_display_properties2)
                functionTable.vkGetDisplayModeProperties2KHR                = reinterpret_cast<PFN_vkGetDisplayModeProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayModeProperties2KHR"));
                functionTable.vkGetDisplayPlaneCapabilities2KHR             = reinterpret_cast<PFN_vkGetDisplayPlaneCapabilities2KHR>(vkGetInstanceProcAddr(instance, "vkGetDisplayPlaneCapabilities2KHR"));
                functionTable.vkGetPhysicalDeviceDisplayPlaneProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayPlaneProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayPlaneProperties2KHR"));
                functionTable.vkGetPhysicalDeviceDisplayProperties2KHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceDisplayProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceDisplayProperties2KHR"));
            #endif
            #if defined(VK_KHR_get_physical_device_properties2)
                functionTable.vkGetPhysicalDeviceFeatures2KHR                    = reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFeatures2KHR"));
                functionTable.vkGetPhysicalDeviceFormatProperties2KHR            = reinterpret_cast<PFN_vkGetPhysicalDeviceFormatProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceFormatProperties2KHR"));
                functionTable.vkGetPhysicalDeviceImageFormatProperties2KHR       = reinterpret_cast<PFN_vkGetPhysicalDeviceImageFormatProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceImageFormatProperties2KHR"));
                functionTable.vkGetPhysicalDeviceMemoryProperties2KHR            = reinterpret_cast<PFN_vkGetPhysicalDeviceMemoryProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceMemoryProperties2KHR"));
                functionTable.vkGetPhysicalDeviceProperties2KHR                  = reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceProperties2KHR"));
                functionTable.vkGetPhysicalDeviceQueueFamilyProperties2KHR       = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyProperties2KHR"));
                functionTable.vkGetPhysicalDeviceSparseImageFormatProperties2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSparseImageFormatProperties2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSparseImageFormatProperties2KHR"));
            #endif
            #if defined(VK_KHR_get_surface_capabilities2)
                functionTable.vkGetPhysicalDeviceSurfaceCapabilities2KHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"));
                functionTable.vkGetPhysicalDeviceSurfaceFormats2KHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormats2KHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormats2KHR"));
            #endif
            #if defined(VK_KHR_performance_query)
                functionTable.vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR = reinterpret_cast<PFN_vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR>(vkGetInstanceProcAddr(instance, "vkEnumeratePhysicalDeviceQueueFamilyPerformanceQueryCountersKHR"));
                functionTable.vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR         = reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceQueueFamilyPerformanceQueryPassesKHR"));
            #endif
            #if defined(VK_KHR_surface)
                functionTable.vkDestroySurfaceKHR                       = reinterpret_cast<PFN_vkDestroySurfaceKHR>(vkGetInstanceProcAddr(instance, "vkDestroySurfaceKHR"));
                functionTable.vkGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
                functionTable.vkGetPhysicalDeviceSurfaceFormatsKHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
                functionTable.vkGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
                functionTable.vkGetPhysicalDeviceSurfaceSupportKHR      = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
            #endif
            #if defined(VK_KHR_video_encode_queue)
                functionTable.vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoEncodeQualityLevelPropertiesKHR"));
            #endif
            #if defined(VK_KHR_video_queue)
                functionTable.vkGetPhysicalDeviceVideoCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoCapabilitiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoCapabilitiesKHR"));
                functionTable.vkGetPhysicalDeviceVideoFormatPropertiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceVideoFormatPropertiesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceVideoFormatPropertiesKHR"));
            #endif
            #if defined(VK_KHR_wayland_surface)
                functionTable.vkCreateWaylandSurfaceKHR                        = reinterpret_cast<PFN_vkCreateWaylandSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWaylandSurfaceKHR"));
                functionTable.vkGetPhysicalDeviceWaylandPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWaylandPresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWaylandPresentationSupportKHR"));
            #endif
            #if defined(VK_KHR_win32_surface)
                functionTable.vkCreateWin32SurfaceKHR                        = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR"));
                functionTable.vkGetPhysicalDeviceWin32PresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));
            #endif
            #if defined(VK_KHR_xcb_surface)
                functionTable.vkCreateXcbSurfaceKHR                        = reinterpret_cast<PFN_vkCreateXcbSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateXcbSurfaceKHR"));
                functionTable.vkGetPhysicalDeviceXcbPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR"));
            #endif
            #if defined(VK_KHR_xlib_surface)
                functionTable.vkCreateXlibSurfaceKHR                        = reinterpret_cast<PFN_vkCreateXlibSurfaceKHR>(vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR"));
                functionTable.vkGetPhysicalDeviceXlibPresentationSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR"));
            #endif
            #if defined(VK_MVK_ios_surface)
                functionTable.vkCreateIOSSurfaceMVK = reinterpret_cast<PFN_vkCreateIOSSurfaceMVK>(vkGetInstanceProcAddr(instance, "vkCreateIOSSurfaceMVK"));
            #endif
            #if defined(VK_MVK_macos_surface)
                functionTable.vkCreateMacOSSurfaceMVK = reinterpret_cast<PFN_vkCreateMacOSSurfaceMVK>(vkGetInstanceProcAddr(instance, "vkCreateMacOSSurfaceMVK"));
            #endif
            #if defined(VK_NN_vi_surface)
                functionTable.vkCreateViSurfaceNN = reinterpret_cast<PFN_vkCreateViSurfaceNN>(vkGetInstanceProcAddr(instance, "vkCreateViSurfaceNN"));
            #endif
            #if defined(VK_NV_acquire_winrt_display)
                functionTable.vkAcquireWinrtDisplayNV = reinterpret_cast<PFN_vkAcquireWinrtDisplayNV>(vkGetInstanceProcAddr(instance, "vkAcquireWinrtDisplayNV"));
                functionTable.vkGetWinrtDisplayNV     = reinterpret_cast<PFN_vkGetWinrtDisplayNV>(vkGetInstanceProcAddr(instance, "vkGetWinrtDisplayNV"));
            #endif
            #if defined(VK_NV_cooperative_matrix)
                functionTable.vkGetPhysicalDeviceCooperativeMatrixPropertiesNV = reinterpret_cast<PFN_vkGetPhysicalDeviceCooperativeMatrixPropertiesNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceCooperativeMatrixPropertiesNV"));
            #endif
            #if defined(VK_NV_coverage_reduction_mode)
                functionTable.vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV = reinterpret_cast<PFN_vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceSupportedFramebufferMixedSamplesCombinationsNV"));
            #endif
            #if defined(VK_NV_external_memory_capabilities)
                functionTable.vkGetPhysicalDeviceExternalImageFormatPropertiesNV = reinterpret_cast<PFN_vkGetPhysicalDeviceExternalImageFormatPropertiesNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceExternalImageFormatPropertiesNV"));
            #endif
            #if defined(VK_NV_optical_flow)
                functionTable.vkGetPhysicalDeviceOpticalFlowImageFormatsNV = reinterpret_cast<PFN_vkGetPhysicalDeviceOpticalFlowImageFormatsNV>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceOpticalFlowImageFormatsNV"));
            #endif
            #if defined(VK_QNX_screen_surface)
                functionTable.vkCreateScreenSurfaceQNX                        = reinterpret_cast<PFN_vkCreateScreenSurfaceQNX>(vkGetInstanceProcAddr(instance, "vkCreateScreenSurfaceQNX"));
                functionTable.vkGetPhysicalDeviceScreenPresentationSupportQNX = reinterpret_cast<PFN_vkGetPhysicalDeviceScreenPresentationSupportQNX>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceScreenPresentationSupportQNX"));
            #endif
            #if (defined(VK_KHR_device_group) && defined(VK_KHR_surface)) || (defined(VK_KHR_swapchain) && defined(VK_VERSION_1_1))
                functionTable.vkGetPhysicalDevicePresentRectanglesKHR = reinterpret_cast<PFN_vkGetPhysicalDevicePresentRectanglesKHR>(vkGetInstanceProcAddr(instance, "vkGetPhysicalDevicePresentRectanglesKHR"));
            #endif
        #pragma endregion

        // Enable validation if supported
         #if SR_ENABLE_LOGGING
            if (validationEnabled) functionTable.vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger);
        #endif
    }

    /* --- GETTER METHODS --- */

    bool VulkanInstance::IsExtensionLoaded(const std::string_view extensionName) const
    {
        return std::ranges::find(loadedExtensions, std::hash<std::string_view>{}(extensionName)) != loadedExtensions.end();
    }

    Version VulkanInstance::GetAPIVersion() const
    {
        // Retrieve version
        uint32 version = VK_API_VERSION_1_0;
        vkEnumerateInstanceVersion(&version);
        version &= 0xFFFFF000;

        #if SR_PLATFORM_APPLE
            // Apple can only translate Vulkan 1.2 through Metal
            if (version >= VK_API_VERSION_1_3)
            {
                version = VK_API_VERSION_1_2;
            }
        #endif

        // Set up version
        const VersionCreateInfo versionCreateInfo
        {
            .major = static_cast<uint8>(VK_API_VERSION_MAJOR(version)),
            .minor = static_cast<uint8>(VK_API_VERSION_MINOR(version)),
            .patch = static_cast<uint8>(VK_API_VERSION_PATCH(version))
        };

        return Version(versionCreateInfo);
    }

    /* --- DESTRUCTOR --- */

    VulkanInstance::~VulkanInstance()
    {
         #if SR_ENABLE_LOGGING
            if (debugMessenger != VK_NULL_HANDLE) functionTable.vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        #endif
        functionTable.vkDestroyInstance(instance, nullptr);
    }

}
