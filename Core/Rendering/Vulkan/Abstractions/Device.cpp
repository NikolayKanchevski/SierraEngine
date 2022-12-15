//
// Created by Nikolay Kanchevski on 11.12.22.
//

#include "Device.h"

#include "../VulkanCore.h"
#include "../../../Version.h"

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Device::Device(DeviceCreateInfo &deviceCreateInfo)
        : requiredFeatures(&deviceCreateInfo.requiredFeatures)
    {
        CreateInstance();
        CreateSurface();

        #if VALIDATION_ENABLED
        CreateDebugMessenger();
        #endif

        RetrievePhysicalDevice();
        CreateLogicalDevice();
        CreateCommandPool();

        RetrieveBestProperties();
    }

    std::shared_ptr<Device> Device::Create(DeviceCreateInfo createInfo)
    {
        return std::make_unique<Device>(createInfo);
    }

    /* --- SETTER METHODS --- */

    void Device::CreateInstance()
    {
        // Create application information
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Sierra Engine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.apiVersion = VK_API_VERSION_1_1;

        // Get GLFW extensions
        uint32_t glfwExtensionCount;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        for (const auto &requiredExtension : requiredInstanceExtensions)
        {
            extensions.push_back(requiredExtension);
        }

        // Check whether all extensions are supported
        ASSERT_WARNING_IF(!ExtensionsSupported(extensions), "Some requested extensions are not supported. They have been automatically disabled, but this could lead to issues");

        // Set up instance creation info
        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.pNext = nullptr;

        // If validation is enabled check validation layers support and bind them to instance
        #if VALIDATION_ENABLED
            ASSERT_ERROR_IF(!ValidationLayersSupported(), "Validation layers requested but are not supported");

            // Set up debug messenger info
            debugMessengerCreateInfo = new VkDebugUtilsMessengerCreateInfoEXT();
            debugMessengerCreateInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            debugMessengerCreateInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            debugMessengerCreateInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debugMessengerCreateInfo->pfnUserCallback = Debugger::ValidationCallback;
            debugMessengerCreateInfo->pUserData = nullptr;

            // Set instance to use the debug messenger
            instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();
            instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) debugMessengerCreateInfo;
        #endif

        // Create instance
        VK_ASSERT(
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance),
            "Could not create Vulkan instance"
        );
    }

    #if VALIDATION_ENABLED
    void Device::CreateDebugMessenger()
    {
        VK_ASSERT(
            CreateDebugUtilsMessengerEXT(instance, debugMessengerCreateInfo, nullptr, &debugMessenger),
            "Failed to create validation messenger"
        );

        delete debugMessengerCreateInfo;
    }
    #endif

    void Device::CreateSurface()
    {
        // Create GLFW surface
        glfwCreateWindowSurface(this->instance, VulkanCore::GetCoreWindow(), nullptr, &surface);
    }

    void Device::RetrievePhysicalDevice()
    {
        // Retrieve how many GPUs are found on the system
        uint32_t physicalDeviceCount;
        VK_ASSERT(
            vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr),
            "Failed to retrieve available GPUs"
        );

        // If none throw error
        ASSERT_ERROR_IF(physicalDeviceCount == 0, "No GPUs found on the system");

        // Put all found GPUs in an array
        VkPhysicalDevice physicalDevices[physicalDeviceCount];
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices);

        // TODO: Pick most suitable GPU, not the first one
        ASSERT_ERROR_IF(physicalDeviceCount <= 0, "No GPUs were found on the system");
        ASSERT_ERROR_IF(!PhysicalDeviceSuitable(physicalDevices[0]), "The GPU of your machine is not supported");

        this->physicalDevice = physicalDevices[0];
        ASSERT_SUCCESS("Vulkan is supported by your [SOME_PC_MODEL] running SOME_OS [Validation: " + std::to_string(VALIDATION_ENABLED) + " | CPU: SOME_CPU_MODEL | GPU: " + physicalDeviceProperties.deviceName + "]");
    }

    void Device::CreateLogicalDevice()
    {
        // Filter out repeating indices using a set
        const std::set<uint32_t> uniqueQueueFamilies { static_cast<uint32_t>(queueFamilyIndices.graphicsFamily), static_cast<uint32_t>(queueFamilyIndices.presentationFamily) };

        // Create an empty list to store create infos
        VkDeviceQueueCreateInfo* queueCreateInfos = new VkDeviceQueueCreateInfo[uniqueQueueFamilies.size()];

        // For each unique family create new create info and add it to the list
        uint32_t i = 0;
        const float queuePriority = 1.0f;
        for (const auto &queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfos[i].queueFamilyIndex = queueFamily;
            queueCreateInfos[i].queueCount = 1;
            queueCreateInfos[i].pQueuePriorities = &queuePriority;
            queueCreateInfos[i].flags = 0;
            queueCreateInfos[i].pNext = nullptr;

            i++;
        }

        // Fill in logical device creation info
        VkDeviceCreateInfo logicalDeviceCreateInfo{};
        logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logicalDeviceCreateInfo.pEnabledFeatures = requiredFeatures;
        logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
        logicalDeviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
        logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(uniqueQueueFamilies.size());
        logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos;

        // Set descriptor indexing features
        #if !__APPLE__
            VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
            if (GetDescriptorIndexingSupported())
            {
                descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
                descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
                descriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
                descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
                logicalDeviceCreateInfo.pNext = &descriptorIndexingFeatures;
            }
        #endif

        // Create logical device
        VK_ASSERT(
            vkCreateDevice(this->physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice),
            "Failed to create logical device"
        );

        // Retrieve queues
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily, 0, &graphicsQueue);
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentationFamily, 0, &presentationQueue);
    }

    void Device::CreateCommandPool()
    {
        // Set up the command pool creation info
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;

        // Create the command pool
        VK_ASSERT(
            vkCreateCommandPool(logicalDevice, &commandPoolCreateInfo, nullptr, &commandPool),
            "Failed to create command pool"
        );
    }

    void Device::RetrieveBestProperties()
    {
        this->bestPresentationMode = ChooseSwapchainPresentMode(swapchainSupportDetails.presentModes);

        this->bestSwapchainImageFormat = ChooseBestSwapchainFormat(swapchainSupportDetails.formats);

        this->bestDepthImageFormat = GetBestDepthBufferFormat(
            { ImageFormat::D32_SFLOAT_S8_UINT, ImageFormat::D32_SFLOAT, ImageFormat::D24_UNORM_S8_UINT },
            ImageTiling::OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

        this->highestMultisampling = GetHighestSupportedSampling();

        // Deallocate useless data
        swapchainSupportDetails.formats.clear();
        swapchainSupportDetails.presentModes.clear();
    }

    /* --- GETTER METHODS --- */

    bool Device::ExtensionsSupported(std::vector<const char*> &givenExtensions)
    {
        // Get how many extensions are supported in total
        uint32_t extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        // Create an array to store the supported extensions
        std::vector<VkExtensionProperties> extensionProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

        // Check if each given extension is in the supported array
        size_t extensionIndex = 0;

        bool success = true;
        for (const auto &requiredExtension : givenExtensions)
        {
            bool extensionFound = false;
            for (const auto &extensionProperty : extensionProperties)
            {
                if (strcmp(requiredExtension, extensionProperty.extensionName) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                success = false;

                ASSERT_WARNING("Instance extension [" + std::string(requiredExtension) + "] not supported");
                givenExtensions.erase(givenExtensions.begin() + extensionIndex);
            }

            extensionIndex++;
        }

        return success;
    }

    #if VALIDATION_ENABLED
    bool Device::ValidationLayersSupported()
    {
        // Get how many validation layers in total are supported
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // Create an array and store the supported layers
        std::vector<VkLayerProperties> layerProperties(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

        // Check if the given layers are in the supported array
        for (const auto &requiredLayer : validationLayers)
        {
            bool layerFound = false;
            for (const auto &layerProperty : layerProperties)
            {
                if (strcmp(requiredLayer, layerProperty.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound)
            {
                return false;
            }
        }

        return true;
    }
    #endif

    bool Device::PhysicalDeviceSuitable(const VkPhysicalDevice &givenPhysicalDevice)
    {
        // Retrieve the GPU's properties
        vkGetPhysicalDeviceProperties(givenPhysicalDevice, &physicalDeviceProperties);

        // Get the features of the given GPU
        vkGetPhysicalDeviceFeatures(givenPhysicalDevice, &physicalDeviceFeatures);

        // Get the queue indices for it and check if they are valid
        queueFamilyIndices = FindQueueFamilies(givenPhysicalDevice);

        bool indicesValid = this->queueFamilyIndices.IsValid();

        // Check if all required extensions are supported
        bool extensionsSupported = DeviceExtensionsSupported(givenPhysicalDevice);

        // Check whether all requested features are supported with this monstrosity oof
        bool featuresSupported =
            requiredFeatures->robustBufferAccess <= physicalDeviceFeatures.robustBufferAccess &&
            requiredFeatures->fullDrawIndexUint32 <= physicalDeviceFeatures.fullDrawIndexUint32 &&
            requiredFeatures->imageCubeArray <= physicalDeviceFeatures.imageCubeArray &&
            requiredFeatures->independentBlend <= physicalDeviceFeatures.independentBlend &&
            requiredFeatures->geometryShader <= physicalDeviceFeatures.geometryShader &&
            requiredFeatures->tessellationShader <= physicalDeviceFeatures.tessellationShader &&
            requiredFeatures->sampleRateShading <= physicalDeviceFeatures.sampleRateShading &&
            requiredFeatures->dualSrcBlend <= physicalDeviceFeatures.dualSrcBlend &&
            requiredFeatures->logicOp <= physicalDeviceFeatures.logicOp &&
            requiredFeatures->multiDrawIndirect <= physicalDeviceFeatures.multiDrawIndirect &&
            requiredFeatures->drawIndirectFirstInstance <= physicalDeviceFeatures.drawIndirectFirstInstance &&
            requiredFeatures->depthClamp <= physicalDeviceFeatures.depthClamp &&
            requiredFeatures->depthBiasClamp <= physicalDeviceFeatures.depthBiasClamp &&
            requiredFeatures->fillModeNonSolid <= physicalDeviceFeatures.fillModeNonSolid &&
            requiredFeatures->depthBounds <= physicalDeviceFeatures.depthBounds &&
            requiredFeatures->wideLines <= physicalDeviceFeatures.wideLines &&
            requiredFeatures->largePoints <= physicalDeviceFeatures.largePoints &&
            requiredFeatures->alphaToOne <= physicalDeviceFeatures.alphaToOne &&
            requiredFeatures->multiViewport <= physicalDeviceFeatures.multiViewport &&
            requiredFeatures->samplerAnisotropy <= physicalDeviceFeatures.samplerAnisotropy &&
            requiredFeatures->textureCompressionETC2 <= physicalDeviceFeatures.textureCompressionETC2 &&
            requiredFeatures->textureCompressionASTC_LDR <= physicalDeviceFeatures.textureCompressionASTC_LDR &&
            requiredFeatures->textureCompressionBC <= physicalDeviceFeatures.textureCompressionBC &&
            requiredFeatures->occlusionQueryPrecise <= physicalDeviceFeatures.occlusionQueryPrecise &&
            requiredFeatures->pipelineStatisticsQuery <= physicalDeviceFeatures.pipelineStatisticsQuery &&
            requiredFeatures->vertexPipelineStoresAndAtomics <= physicalDeviceFeatures.vertexPipelineStoresAndAtomics &&
            requiredFeatures->fragmentStoresAndAtomics <= physicalDeviceFeatures.fragmentStoresAndAtomics &&
            requiredFeatures->shaderTessellationAndGeometryPointSize <= physicalDeviceFeatures.shaderTessellationAndGeometryPointSize &&
            requiredFeatures->shaderImageGatherExtended <= physicalDeviceFeatures.shaderImageGatherExtended &&
            requiredFeatures->shaderStorageImageExtendedFormats <= physicalDeviceFeatures.shaderStorageImageExtendedFormats &&
            requiredFeatures->shaderStorageImageMultisample <= physicalDeviceFeatures.shaderStorageImageMultisample &&
            requiredFeatures->shaderStorageImageReadWithoutFormat <= physicalDeviceFeatures.shaderStorageImageReadWithoutFormat &&
            requiredFeatures->shaderStorageImageWriteWithoutFormat <= physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat &&
            requiredFeatures->shaderUniformBufferArrayDynamicIndexing <= physicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing &&
            requiredFeatures->shaderSampledImageArrayDynamicIndexing <= physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing &&
            requiredFeatures->shaderStorageBufferArrayDynamicIndexing <= physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing &&
            requiredFeatures->shaderStorageImageArrayDynamicIndexing <= physicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing &&
            requiredFeatures->shaderClipDistance <= physicalDeviceFeatures.shaderClipDistance &&
            requiredFeatures->shaderCullDistance <= physicalDeviceFeatures.shaderCullDistance &&
            requiredFeatures->shaderFloat64 <= physicalDeviceFeatures.shaderFloat64 &&
            requiredFeatures->shaderInt64 <= physicalDeviceFeatures.shaderInt64 &&
            requiredFeatures->shaderInt16 <= physicalDeviceFeatures.shaderInt16 &&
            requiredFeatures->shaderResourceResidency <= physicalDeviceFeatures.shaderResourceResidency &&
            requiredFeatures->shaderResourceMinLod <= physicalDeviceFeatures.shaderResourceMinLod &&
            requiredFeatures->sparseBinding <= physicalDeviceFeatures.sparseBinding &&
            requiredFeatures->sparseResidencyBuffer <= physicalDeviceFeatures.sparseResidencyBuffer &&
            requiredFeatures->sparseResidencyImage2D <= physicalDeviceFeatures.sparseResidencyImage2D &&
            requiredFeatures->sparseResidencyImage3D <= physicalDeviceFeatures.sparseResidencyImage3D &&
            requiredFeatures->sparseResidency2Samples <= physicalDeviceFeatures.sparseResidency2Samples &&
            requiredFeatures->sparseResidency4Samples <= physicalDeviceFeatures.sparseResidency4Samples &&
            requiredFeatures->sparseResidency8Samples <= physicalDeviceFeatures.sparseResidency8Samples &&
            requiredFeatures->sparseResidency16Samples <= physicalDeviceFeatures.sparseResidency16Samples &&
            requiredFeatures->sparseResidencyAliased <= physicalDeviceFeatures.sparseResidencyAliased &&
            requiredFeatures->variableMultisampleRate <= physicalDeviceFeatures.variableMultisampleRate &&
            requiredFeatures->inheritedQueries <= physicalDeviceFeatures.inheritedQueries;

        swapchainSupportDetails = GetSwapchainSupportDetails(givenPhysicalDevice);

        bool swapchainAdequate = !swapchainSupportDetails.formats.empty() && !swapchainSupportDetails.presentModes.empty();

        return indicesValid && extensionsSupported && swapchainAdequate && featuresSupported;
    }

    bool Device::DeviceExtensionsSupported(VkPhysicalDevice const &givenPhysicalDevice)
    {
        // Get how many extensions are supported in total
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(givenPhysicalDevice, nullptr, &extensionCount, nullptr);

        // Create an array to store the supported extensions
        std::vector<VkExtensionProperties> extensionProperties(extensionCount);
        vkEnumerateDeviceExtensionProperties(givenPhysicalDevice, nullptr, &extensionCount, extensionProperties.data());

        #if __APPLE__
            bool khrPortabilityRequired = false;
        #endif

        // Check if each given extension is in the supported extensions array
        bool allExtensionsSupported = true;
        for (const auto &requiredExtension : requiredDeviceExtensions)
        {
            bool extensionFound = false;
            for (const auto &extensionProperty : extensionProperties)
            {
                #if __APPLE__
                    // Check if Mac's specific extension is supported (required if so)
                    if (strcmp("VK_KHR_portability_subset", extensionProperty.extensionName) == 0)
                    {
                        khrPortabilityRequired = true;
                        continue;
                    }
                #endif

                if (strcmp(requiredExtension, extensionProperty.extensionName) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                allExtensionsSupported = false;
                ASSERT_WARNING("Device extension [" + std::string(requiredExtension) + "] not supported by your [" + std::string(VulkanCore::GetPhysicalDeviceProperties().deviceName) + "] GPU");
            }
        }

        #if __APPLE__
            if (khrPortabilityRequired) requiredDeviceExtensions.push_back("VK_KHR_portability_subset");
        #endif

        if (GetDescriptorIndexingSupported()) requiredDeviceExtensions.push_back("VK_EXT_descriptor_indexing");

        return allExtensionsSupported;
    }

    Device::QueueFamilyIndices Device::FindQueueFamilies(const VkPhysicalDevice &givenPhysicalDevice)
    {
        QueueFamilyIndices indices{};

        // Get how many family properties are available
        uint32_t queueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(givenPhysicalDevice, &queueFamilyPropertiesCount, nullptr);

        // Put each of them in an array
        VkQueueFamilyProperties queueFamilyProperties[queueFamilyPropertiesCount];
        vkGetPhysicalDeviceQueueFamilyProperties(givenPhysicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties);

        // Iterate trough each
        for (uint32_t i = queueFamilyPropertiesCount; i--;)
        {
            // Save the current one
            VkQueueFamilyProperties currentQueueProperties = queueFamilyProperties[i];

            // Check if the current queue has a graphics family
            if ((currentQueueProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                indices.graphicsFamily = i;
            }

            // Check if the current queue supports presentation
            VkBool32 presentationSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(givenPhysicalDevice, i, this->surface, &presentationSupported);

            // If so set its presentation family
            if (presentationSupported)
            {
                indices.presentationFamily = i;
            }

            // If the indices are already valid there's no need to continue the loop
            if (indices.IsValid())
            {
                break;
            }
        }

        return indices;
    }

    Device::SwapchainSupportDetails Device::GetSwapchainSupportDetails(const VkPhysicalDevice &givenPhysicalDevice)
    {
        // Get the details of the GPU's supported swapchain
        SwapchainSupportDetails swapchainDetails{};
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(givenPhysicalDevice, surface, &swapchainDetails.capabilities);

        // Get how many formats are available
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(givenPhysicalDevice, this->surface, &formatCount, nullptr);

        // Put each of them in an array
        swapchainDetails.formats = std::vector<VkSurfaceFormatKHR>(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(givenPhysicalDevice, this->surface, &formatCount, swapchainDetails.formats.data());

        // Get how many presentation modes are available
        uint32_t presentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(givenPhysicalDevice, this->surface, &presentModesCount, nullptr);

        // Put each of them in an array
        swapchainDetails.presentModes = std::vector<VkPresentModeKHR>(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(givenPhysicalDevice, this->surface, &formatCount, swapchainDetails.presentModes.data());

        return swapchainDetails;
    }

    VkSurfaceFormatKHR Device::ChooseBestSwapchainFormat(std::vector<VkSurfaceFormatKHR> &givenFormats)
    {
        for (const auto &availableFormat : givenFormats)
        {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return availableFormat;
            }
        }

        // Otherwise just return the very first one
        return givenFormats[0];
    }

    ImageFormat Device::GetBestDepthBufferFormat(std::vector<ImageFormat> givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags)
    {
        for (const auto &givenFormat : givenFormats)
        {
            // Get the properties for the current format
            VkFormatProperties formatProperties;
            vkGetPhysicalDeviceFormatProperties(physicalDevice, (VkFormat) givenFormat, &formatProperties);

            // Check if the required format properties are supported
            if (imageTiling == ImageTiling::LINEAR && (formatProperties.linearTilingFeatures & formatFeatureFlags) == formatFeatureFlags)
            {
                return givenFormat;
            }
            else if (imageTiling == ImageTiling::OPTIMAL && (formatProperties.optimalTilingFeatures & formatFeatureFlags) == formatFeatureFlags)
            {
                return givenFormat;
            }
        }

        // Otherwise throw an error
        ASSERT_ERROR("No depth buffer formats supported");

        return ImageFormat::UNDEFINED;
    }

    VkPresentModeKHR Device::ChooseSwapchainPresentMode(std::vector<VkPresentModeKHR> &givenPresentModes)
    {
        // Loop trough each to check if it is VK_PRESENT_MODE_MAILBOX_KHR
        for (const auto &availablePresentMode : givenPresentModes)
        {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                return availablePresentMode;
            }
        }

        // Otherwise return VK_PRESENT_MODE_FIFO_KHR which is guaranteed to be available
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    Multisampling Device::GetHighestSupportedSampling()
    {
        VkSampleCountFlags countFlags = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

        if ((countFlags & (int) Multisampling::MSAAx64) != 0) return Multisampling::MSAAx64;
        if ((countFlags & (int) Multisampling::MSAAx32) != 0) return Multisampling::MSAAx32;
        if ((countFlags & (int) Multisampling::MSAAx16) != 0) return Multisampling::MSAAx16;
        if ((countFlags & (int) Multisampling::MSAAx8) != 0) return Multisampling::MSAAx8;
        if ((countFlags & (int) Multisampling::MSAAx4) != 0) return Multisampling::MSAAx4;
        if ((countFlags & (int) Multisampling::MSAAx2) != 0) return Multisampling::MSAAx2;

        return Multisampling::MSAAx1;
    }

    /* --- DESTRUCTOR --- */

    void Device::Destroy()
    {
        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);

        vkDestroyDevice(logicalDevice, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);

        #if VALIDATION_ENABLED
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        #endif

        vkDestroyInstance(instance, nullptr);
    }
}



/* --- POLLING METHODS --- */

/* --- SETTER METHODS --- */

/* --- GETTER METHODS --- */


