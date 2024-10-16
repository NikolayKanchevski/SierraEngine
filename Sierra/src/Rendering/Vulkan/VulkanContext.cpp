//
// Created by Nikolay Kanchevski on 9.09.23.
//

#include <vulkan/vulkan.h>
#include "VulkanContext.h"

#include "VulkanDevice.h"
#include "VulkanErrorHandler.h"

namespace Sierra
{

    namespace
    {
        struct VulkanInstanceLayer
        {
            std::string_view name;
            bool optional = false;
        };

        constexpr std::array<VulkanInstanceLayer, SR_ENABLE_LOGGING> REQUESTED_INSTANCE_LAYERS
        {
            #if SR_ENABLE_LOGGING
                VulkanInstanceLayer
                {
                    .name = "VK_LAYER_KHRONOS_validation",
                    .optional = true
                }
            #endif
        };

        struct VulkanInstanceExtension
        {
            std::string_view name;
            bool optional = false;
        };

        constexpr std::array REQUESTED_INSTANCE_EXTENSIONS
        {
            VulkanInstanceExtension
            {
                .name = VK_KHR_SURFACE_EXTENSION_NAME
            },
            VulkanInstanceExtension
            {
                .name = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME
            },
            #if SR_ENABLE_LOGGING
                VulkanInstanceExtension
                {
                    .name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                    .optional = true
                },
            #endif
            #if SR_PLATFORM_WINDOWS
                VulkanInstanceExtension
                {
                    .name = VK_KHR_WIN32_SURFACE_EXTENSION_NAME
                }
            #elif SR_PLATFORM_LINUX
                VulkanInstanceExtension
                {
                    .name = VK_KHR_XLIB_SURFACE_EXTENSION_NAME
                }
            #elif SR_PLATFORM_ANDROID
                VulkanInstanceExtension
                {
                    .name = VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
                }
            #elif SR_PLATFORM_APPLE
                VulkanInstanceExtension
                {
                    .name = VK_EXT_METAL_SURFACE_EXTENSION_NAME
                },
                VulkanInstanceExtension
                {
                    .name = VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME,
                    .optional = true
                }
            #endif
        };

        Version GetVulkanVersion()
        {
            // Retrieve version
            uint32 version = VK_API_VERSION_1_0;
            vkEnumerateInstanceVersion(&version);
            version &= 0xFFFFF000;

            #if SR_PLATFORM_APPLE
                // Apple can only translate Vulkan 1.2 through Vulkan
                if (version >= VK_API_VERSION_1_3)
                {
                    version = VK_API_VERSION_1_2;
                }
            #endif

            return Version({ VK_API_VERSION_MAJOR(version), VK_API_VERSION_MINOR(version), VK_API_VERSION_PATCH(version) });
        }

        struct VulkanDeviceExtension
        {
            std::string_view name;
            bool optional = false;
            void* data = nullptr;
        };

        #if SR_PLATFORM_APPLE
            VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubsetFeatures
            {
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
                .imageViewFormatSwizzle = true
            };
        #endif

        VkPhysicalDeviceImagelessFramebufferFeaturesKHR imagelessFramebufferFeatures
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_IMAGELESS_FRAMEBUFFER_FEATURES_KHR,
            .imagelessFramebuffer = VK_TRUE
        };

        VkPhysicalDeviceDescriptorIndexingFeaturesEXT descriptorIndexingFeatures
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT,
            .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
            .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
            .descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
            .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
            .descriptorBindingPartiallyBound = VK_TRUE,
            .runtimeDescriptorArray = VK_TRUE
        };

        VkPhysicalDeviceTimelineSemaphoreFeaturesKHR timelineSemaphoreFeatures
        {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES_KHR,
            .timelineSemaphore = VK_TRUE
        };

        constexpr std::array REQUESTED_DEVICE_EXTENSIONS
        {
            VulkanDeviceExtension
            {
                // Core in Vulkan 1.0
                .name = VK_KHR_SWAPCHAIN_EXTENSION_NAME
            },
            VulkanDeviceExtension
            {
                // Core in Vulkan 1.1
                .name = VK_KHR_MAINTENANCE_1_EXTENSION_NAME
            },
            VulkanDeviceExtension
            {
                // Core in Vulkan 1.2 (dependency of VK_KHR_imageless_framebuffer)
                .name = VK_KHR_IMAGE_FORMAT_LIST_EXTENSION_NAME,
            },
            VulkanDeviceExtension
            {
                // Core in Vulkan 1.2
                .name = VK_KHR_IMAGELESS_FRAMEBUFFER_EXTENSION_NAME,
                .data = &imagelessFramebufferFeatures
            },
            VulkanDeviceExtension
            {
                // Core in Vulkan 1.2
                .name = VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                .data = &descriptorIndexingFeatures
            },
            VulkanDeviceExtension
            {
                // Core in Vulkan 1.2
                .name = VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
                .data = &timelineSemaphoreFeatures
            },
            #if SR_ENABLE_LOGGING
                VulkanDeviceExtension
                {
                    .name = VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                    .optional = true
                },
            #endif
            #if SR_PLATFORM_APPLE
                VulkanDeviceExtension
                {
                    .name = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
                    .optional = true,
                    .data = &portabilitySubsetFeatures
                },
            #endif
        };
    }

    /* --- CONSTRUCTORS --- */

    VulkanContext::VulkanContext(const RenderingContextCreateInfo& createInfo)
        : RenderingContext(createInfo), name(createInfo.name), vulkanVersion(GetVulkanVersion())
    {
        // Optional MoltenVK features must be explicitly enabled prior to performing any API calls
        #if SR_PLATFORM_APPLE
            #if SR_ENABLE_LOGGING
                setenv("MVK_DEBUG", "1", true);
            #endif
            setenv("MVK_CONFIG_USE_METAL_ARGUMENT_BUFFERS", "2", true);
        #endif

        // Set up application info
        const VkApplicationInfo applicationInfo
        {
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pApplicationName = createInfo.applicationName.data(),
            .applicationVersion = VK_MAKE_API_VERSION(0, createInfo.applicationVersion.GetMajor(), createInfo.applicationVersion.GetMinor(), createInfo.applicationVersion.GetPatch()),
            .pEngineName = "Sierra Engine",
            .engineVersion = VK_MAKE_API_VERSION(0, SR_VERSION_MAJOR, SR_VERSION_MINOR, SR_VERSION_PATCH),
            .apiVersion = VK_MAKE_API_VERSION(0, vulkanVersion.GetMajor(), vulkanVersion.GetMinor(), vulkanVersion.GetPatch())
        };

        // Define extensions array
        uint32 extensionCount = 0;
        std::array<const char*, REQUESTED_INSTANCE_EXTENSIONS.size()> extensions = { };

        bool debugUtilsExtensionEnabled = false;
        bool portabilityEnumerationExtensionEnabled = false;
        if (!REQUESTED_INSTANCE_EXTENSIONS.empty())
        {
            // Retrieve supported extension count
            uint32 supportedExtensionCount = 0;
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr);

            // Retrieve supported extensions
            std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, supportedExtensions.data());

            // Load extensions
            for (const VulkanInstanceExtension& requestedExtension : REQUESTED_INSTANCE_EXTENSIONS)
            {
                bool extensionFound = false;
                for (const VkExtensionProperties& supportedExtension : supportedExtensions)
                {
                    if (strcmp(requestedExtension.name.data(), supportedExtension.extensionName) == 0)
                    {
                        extensions[extensionCount] = requestedExtension.name.data();
                        extensionCount++;

                        debugUtilsExtensionEnabled = debugUtilsExtensionEnabled || requestedExtension.name == VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
                        portabilityEnumerationExtensionEnabled = portabilityEnumerationExtensionEnabled || requestedExtension.name == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;

                        extensionFound = true;
                        break;
                    }
                }

                SR_THROW_IF(!extensionFound && !requestedExtension.optional, UnsupportedFeatureError(SR_FORMAT("Cannot create rendering context [{0}], as required Vulkan instance extension [{1}] is unsupported", name, requestedExtension.name)));
            }
        }

        // Define layers array
        uint32 layerCount = 0;
        std::array<const char*, REQUESTED_INSTANCE_LAYERS.size()> layers = { };

        if (!REQUESTED_INSTANCE_LAYERS.empty())
        {
            // Retrieve supported layer count
            uint32 supportedLayerCount = 0;
            vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr);

            // Retrieve supported layers
            std::vector<VkLayerProperties> supportedLayers(supportedLayerCount);
            vkEnumerateInstanceLayerProperties(&supportedLayerCount, supportedLayers.data());

            // Load layers
            for (const VulkanInstanceLayer& requestedLayer : REQUESTED_INSTANCE_LAYERS)
            {
                bool layerFound = false;
                for (const VkLayerProperties& supportedLayer : supportedLayers)
                {
                    if (strcmp(requestedLayer.name.data(), supportedLayer.layerName) == 0)
                    {
                        layers[layerCount] = requestedLayer.name.data();
                        layerCount++;

                        layerFound = true;
                        break;
                    }
                }

                SR_THROW_IF(!layerFound && !requestedLayer.optional, UnsupportedFeatureError(SR_FORMAT("Cannot create rendering context [{0}], as required Vulkan instance layer [{1}] is unsupported", name, requestedLayer.name)));
            }
        }

        // Set up instance create info
        VkInstanceCreateInfo instanceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = static_cast<uint32>(portabilityEnumerationExtensionEnabled) * VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
            .pApplicationInfo = &applicationInfo,
            .enabledLayerCount = layerCount,
            .ppEnabledLayerNames = layers.data(),
            .enabledExtensionCount = extensionCount,
            .ppEnabledExtensionNames = extensions.data()
        };

        #if SR_ENABLE_LOGGING
            VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = { };
            if (debugUtilsExtensionEnabled)
            {
                debugMessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
                debugMessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
                debugMessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
                debugMessengerCreateInfo.pfnUserCallback = [](const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*) -> VkBool32
                {
                    switch (messageSeverity)
                    {
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                        {
                            SR_WARNING("Vulkan {0}", callbackData->pMessage);
                            break;
                        }
                        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                        {
                            SR_ERROR("Vulkan {0}", callbackData->pMessage);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }

                    return VK_FALSE;
                };

                AddToPNextChain(&instanceCreateInfo, &debugMessengerCreateInfo);
            }
        #endif

        // Create instance
        VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &instance);
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create rendering context [{0}]", name));

        // Save loaded extensions
        loadedExtensions.resize(extensionCount);
        for (size i = 0; i < extensionCount; i++)
        {
            loadedExtensions[i] = std::hash<std::string_view>{}(extensions[i]);
        }

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

        #if SR_ENABLE_LOGGING
            if (debugUtilsExtensionEnabled)
            {
                result = functionTable.vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger);
                SR_WARNING_IF(result != VK_SUCCESS, "Could not create debug messenger for rendering context [{0}], despite required Vulkan extension being supported", name);
            }
        #endif
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Device> VulkanContext::CreateDevice(const DeviceCreateInfo& createInfo) const
    {
        // Retrieve number of GPUs found
        uint32 physicalDeviceCount = 0;
        VkResult result = functionTable.vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Rendering context [{0}] could not create requested device", name));
        SR_THROW_IF(physicalDeviceCount <= 0, UnsupportedFeatureError(SR_FORMAT("Cannot create requested device, as rendering context [{0}] could not find any supported devices", name)));

        // Retrieve GPUs
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        functionTable.vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

        for (VkPhysicalDevice physicalDevice : physicalDevices)
        {
            // Define extensions
            uint32 extensionCount = 0;
            std::array<const char*, REQUESTED_DEVICE_EXTENSIONS.size()> extensions = { };

            // Set up device features
            VkPhysicalDeviceFeatures2 physicalDeviceFeatures { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2 };
            functionTable.vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures.features);

            // Define extensions array
            if (!REQUESTED_DEVICE_EXTENSIONS.empty())
            {
                // Retrieve supported extension count
                uint32 supportedExtensionCount = 0;
                functionTable.vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, nullptr);

                // Retrieve supported extensions
                std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
                functionTable.vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, supportedExtensions.data());

                // Load extensions
                bool requiredExtensionsEnabled = true;
                for (const VulkanDeviceExtension& requestedExtension : REQUESTED_DEVICE_EXTENSIONS)
                {
                    bool extensionFound = false;
                    for (const VkExtensionProperties& supportedExtension : supportedExtensions)
                    {
                        if (strcmp(requestedExtension.name.data(), supportedExtension.extensionName) == 0)
                        {
                            extensions[extensionCount] = requestedExtension.name.data();
                            extensionCount++;

                            if (requestedExtension.data != nullptr)
                            {
                                AddToPNextChain(&physicalDeviceFeatures, requestedExtension.data);
                            }

                            extensionFound = true;
                            break;
                        }
                    }

                    // If extension is unsupported but required discard device as a viable option
                    if (!extensionFound && !requestedExtension.optional)
                    {
                        requiredExtensionsEnabled = false;
                        break;
                    }
                }

                if (!requiredExtensionsEnabled) continue;
            }

            // Retrieve queue family properties count
            uint32 queueFamilyPropertiesCount = 0;
            functionTable.vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, nullptr);

            // Retrieve queue family properties
            std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
            functionTable.vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

            uint32 queueCount = 0;
            std::array<VulkanQueueDescription, 10> queueDescriptions = { };

            // Find out queues for device
            bool graphicsQueueFound = false;
            bool computeQueueFound = false;
            bool transferQueueFound = false;
            for (size i = 0; i < queueFamilyProperties.size(); i++)
            {
                VulkanQueueDescription queueInfo
                {
                    .family = static_cast<uint32>(i),
                    .operations = QueueOperations::None
                };

                // NOTE: Spec says, that graphics and compute support implies support for transfer operations too
                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    queueInfo.operations |= (QueueOperations::Graphics | QueueOperations::Transfer);
                    graphicsQueueFound = true;
                }
                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
                {
                    queueInfo.operations |= (QueueOperations::Compute | QueueOperations::Transfer);
                    computeQueueFound = true;
                }
                if (queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
                {
                    queueInfo.operations |= QueueOperations::Transfer;
                    transferQueueFound = true;
                }

                // If queue does not serve a useful purpose, discard it
                if (queueInfo.operations == QueueOperations::None) continue;

                queueDescriptions[i] = queueInfo;
                queueCount++;
            }

            // Discard device if no queues are found to support graphics, compute, and transfer operations
            if (!graphicsQueueFound || !computeQueueFound || !transferQueueFound) continue;

            // Create device and get its limits
            std::unique_ptr<Device> device = std::make_unique<VulkanDevice>(*this, physicalDevice, std::span(extensions.data(), extensionCount), std::span(queueDescriptions.data(), queueCount), &physicalDeviceFeatures, createInfo);
            const DeviceLimits deviceLimits = device->GetLimits();

            // Check if device meets requirements
            if (
                createInfo.requirements.limits.maxUniformBufferSize > deviceLimits.maxUniformBufferSize                              ||
                createInfo.requirements.limits.maxStorageBufferSize > deviceLimits.maxStorageBufferSize                              ||
                createInfo.requirements.limits.maxLineImageDimensions > deviceLimits.maxLineImageDimensions                          ||
                createInfo.requirements.limits.maxPlaneImageDimensions > deviceLimits.maxPlaneImageDimensions                        ||
                createInfo.requirements.limits.maxVolumeImageDimensions > deviceLimits.maxVolumeImageDimensions                      ||
                createInfo.requirements.limits.maxCubeImageDimensions > deviceLimits.maxCubeImageDimensions                          ||
                createInfo.requirements.limits.resourceTableUniformBufferCapacity > deviceLimits.resourceTableUniformBufferCapacity  ||
                createInfo.requirements.limits.resourceTableStorageBufferCapacity > deviceLimits.resourceTableStorageBufferCapacity  ||
                createInfo.requirements.limits.resourceTableSampledImageCapacity > deviceLimits.resourceTableSampledImageCapacity    ||
                createInfo.requirements.limits.resourceTableStorageImageCapacity > deviceLimits.resourceTableStorageImageCapacity    ||
                createInfo.requirements.limits.resourceTableSamplerCapacity > deviceLimits.resourceTableSamplerCapacity              ||
                createInfo.requirements.limits.maxRenderPassWidth > deviceLimits.maxRenderPassWidth                                  ||
                createInfo.requirements.limits.maxRenderPassHeight > deviceLimits.maxRenderPassHeight                                ||
                createInfo.requirements.limits.maxWorkGroupSize.x > deviceLimits.maxWorkGroupSize.x                                  ||
                createInfo.requirements.limits.maxWorkGroupSize.y > deviceLimits.maxWorkGroupSize.y                                  ||
                createInfo.requirements.limits.maxWorkGroupSize.z > deviceLimits.maxWorkGroupSize.z                                  ||
                createInfo.requirements.limits.highestImageSampling > deviceLimits.highestImageSampling                              ||
                createInfo.requirements.limits.highestSamplerAnisotropy > deviceLimits.highestSamplerAnisotropy
            )
            {
                continue;
            }

            return device;
        }

        SR_THROW(UnsupportedFeatureError(SR_FORMAT("Rendering context [{0}] failed to create device [{1}], as no supported GPU, which matches the specified requirements was found on the system", name, createInfo.name)));
        return nullptr;
    }

    /* --- DESTRUCTOR --- */

    VulkanContext::~VulkanContext() noexcept
    {
        #if SR_ENABLE_LOGGING
            if (debugMessenger != VK_NULL_HANDLE) functionTable.vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        #endif
        functionTable.vkDestroyInstance(instance, nullptr);
    }

}
