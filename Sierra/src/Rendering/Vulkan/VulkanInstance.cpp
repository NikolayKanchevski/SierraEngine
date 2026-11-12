//
// Created by Nikolay Kanchevski on 9.09.23.
//

#include <vulkan/vulkan.h>
#include "VulkanInstance.h"

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
                // Windowing backend is only known at runtime, so either (or both) of these may be loaded
                VulkanInstanceExtension
                {
                    .name = VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
                    .optional = true
                },
                VulkanInstanceExtension
                {
                    .name = VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
                    .optional = true
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

            return Version({ static_cast<uint8>(VK_API_VERSION_MAJOR(version)), static_cast<uint8>(VK_API_VERSION_MINOR(version)), static_cast<uint8>(VK_API_VERSION_PATCH(version)) });
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
                .name = VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
                .data = &descriptorIndexingFeatures
            },
            VulkanDeviceExtension
            {
                // Core in Vulkan 1.2
                .name = VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME,
                .data = &timelineSemaphoreFeatures
            },
            VulkanDeviceExtension {
                // Core in Vulkan 1.1
                .name = VK_KHR_DEDICATED_ALLOCATION_EXTENSION_NAME,
                .optional = true
            },
            VulkanDeviceExtension {
                // Core in Vulkan 1.1
                .name = VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
                .optional = true
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

    VulkanInstance::VulkanInstance(const RenderingInstanceCreateInfo& createInfo)
        : RenderingInstance(createInfo), name(createInfo.name), vulkanVersion(GetVulkanVersion())
    {
        // Optional MoltenVK features must be explicitly enabled prior to performing any API calls
        #if SR_PLATFORM_APPLE
            #if SR_ENABLE_LOGGING
                setenv("MVK_DEBUG", "1", true);
                // setenv("MVK_CONFIG_DEBUG", "1", true);
                // setenv("MVK_CONFIG_LOG_LEVEL", "4", true);
                // setenv("MVK_CONFIG_TRACE_VULKAN_CALLS", "1", true);
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
                        #if defined(VK_KHR_portability_enumeration)
                            portabilityEnumerationExtensionEnabled = portabilityEnumerationExtensionEnabled || requestedExtension.name == VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME;
                        #endif

                        extensionFound = true;
                        break;
                    }
                }

                SR_THROW_IF(!extensionFound && !requestedExtension.optional, UnsupportedFeatureError(SR_FORMAT("Cannot create rendering context [{0}], as required Vulkan instance extension [{1}] is unsupported", createInfo.name, requestedExtension.name)));
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

                SR_THROW_IF(!layerFound && !requestedLayer.optional, UnsupportedFeatureError(SR_FORMAT("Cannot create rendering context [{0}], as required Vulkan instance layer [{1}] is unsupported", createInfo.name, requestedLayer.name)));
            }
        }

        // Set up instance create info
        VkInstanceCreateInfo instanceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            #if defined(VK_KHR_portability_enumeration)
                .flags = static_cast<uint32>(portabilityEnumerationExtensionEnabled) * VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR,
            #endif
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
                debugMessengerCreateInfo.pfnUserCallback = [](const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, const VkDebugUtilsMessageTypeFlagsEXT, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void*) VKAPI_PTR -> VkBool32
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
        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Could not create rendering context [{0}]", createInfo.name));

        // Save loaded extensions
        loadedExtensions.resize(extensionCount);
        for (size i = 0; i < extensionCount; i++)
        {
            loadedExtensions[i] = std::hash<std::string_view>{}(extensions[i]);
        }

        // Load function pointers
        functionTable = LoadVulkanInstanceFunctionTable(instance);

        #if SR_ENABLE_LOGGING
            if (debugUtilsExtensionEnabled)
            {
                result = functionTable.vkCreateDebugUtilsMessengerEXT(instance, &debugMessengerCreateInfo, nullptr, &debugMessenger);
                SR_WARNING_IF(result != VK_SUCCESS, "Could not create debug messenger for rendering context [{0}], despite required Vulkan extension being supported", createInfo.name);
            }
        #endif
    }

    /* --- POLLING METHODS --- */

    std::unique_ptr<Device> VulkanInstance::CreateDevice(const DeviceCreateInfo& createInfo) const
    {
        // Retrieve number of GPUs found
        uint32 physicalDeviceCount = 0;
        VkResult result = functionTable.vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

        if (result != VK_SUCCESS) HandleVulkanError(result, SR_FORMAT("Rendering context [{0}] could not create requested device", createInfo.name));
        SR_THROW_IF(physicalDeviceCount <= 0, UnsupportedFeatureError(SR_FORMAT("Cannot create requested device, as rendering context [{0}] could not find any supported devices", createInfo.name)));

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
                createInfo.requirements.limits.maxStorageBufferSize > deviceLimits.maxStorageBufferSize ||
                createInfo.requirements.limits.maxLineImageDimensions > deviceLimits.maxLineImageDimensions ||
                createInfo.requirements.limits.maxPlaneImageDimensions > deviceLimits.maxPlaneImageDimensions ||
                createInfo.requirements.limits.maxVolumeImageDimensions > deviceLimits.maxVolumeImageDimensions ||
                createInfo.requirements.limits.maxCubeImageDimensions > deviceLimits.maxCubeImageDimensions ||
                createInfo.requirements.limits.resourceTableUniformBufferCapacity > deviceLimits.resourceTableUniformBufferCapacity ||
                createInfo.requirements.limits.resourceTableStorageBufferCapacity > deviceLimits.resourceTableStorageBufferCapacity ||
                createInfo.requirements.limits.resourceTableSampledImageCapacity > deviceLimits.resourceTableSampledImageCapacity ||
                createInfo.requirements.limits.resourceTableStorageImageCapacity > deviceLimits.resourceTableStorageImageCapacity ||
                createInfo.requirements.limits.resourceTableSamplerCapacity > deviceLimits.resourceTableSamplerCapacity ||
                createInfo.requirements.limits.maxFramebufferWidth > deviceLimits.maxFramebufferWidth ||
                createInfo.requirements.limits.maxFramebufferHeight > deviceLimits.maxFramebufferHeight ||
                createInfo.requirements.limits.maxWorkGroupSize.x > deviceLimits.maxWorkGroupSize.x ||
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

        SR_THROW(UnsupportedFeatureError(SR_FORMAT("Rendering context [{0}] failed to create device [{1}], as no supported GPU, which matches the specified requirements was found on the system", createInfo.name, createInfo.name)));
        return nullptr;
    }

    /* --- DESTRUCTOR --- */

    VulkanInstance::~VulkanInstance() noexcept
    {
        #if SR_ENABLE_LOGGING
            if (debugMessenger != VK_NULL_HANDLE) functionTable.vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        #endif
        functionTable.vkDestroyInstance(instance, nullptr);
    }

}
