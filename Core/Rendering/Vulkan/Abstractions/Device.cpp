//
// Created by Nikolay Kanchevski on 11.12.22.
//

#include "Device.h"

#include "../VK.h"
#include "../../../Engine/Classes/SystemInformation.h"

#define DEBUG_DEVICE_EXTENSIONS 0

using Sierra::Engine::Classes::SystemInformation;

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{
    /* --- CONSTRUCTORS --- */

    Device::Device(DeviceCreateInfo &deviceCreateInfo)
        : requiredFeatures(&deviceCreateInfo.requiredFeatures)
    {
        RetrievePhysicalDevice();
        CreateLogicalDevice();
        RetrieveBestProperties();

        // Connect device with Volk
        volkLoadDevice(logicalDevice);
    }

    UniquePtr<Device> Device::Create(DeviceCreateInfo createInfo)
    {
        return std::make_unique<Device>(createInfo);
    }

    /* --- GETTER METHODS --- */

    UniquePtr<CommandBuffer> Device::BeginSingleTimeCommands() const
    {
        UniquePtr<CommandBuffer> commandBuffer = CommandBuffer::Create();
        commandBuffer->Begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        return std::move(commandBuffer);
    }

    void Device::EndSingleTimeCommands(UniquePtr<CommandBuffer> &commandBuffer) const
    {
        commandBuffer->End();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;

        VkCommandBuffer commandBufferPtr = commandBuffer->GetVulkanCommandBuffer();
        submitInfo.pCommandBuffers = &commandBufferPtr;

        vkQueueSubmit(VK::GetDevice()->GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(VK::GetDevice()->GetGraphicsQueue());

        commandBuffer->Free();
    }

    /* --- SETTER METHODS --- */


    void Device::RetrievePhysicalDevice()
    {
        // Set up example surface parameters
        glfwWindowHint(GLFW_VISIBLE, 0);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

        // Create the example surface
        GLFWwindow* glfwWindow = glfwCreateWindow(100, 100, "dQw4w9WgXcQ", nullptr, nullptr);
        glfwCreateWindowSurface(VK::GetInstance(), glfwWindow, nullptr, &exampleSurface);

        // Retrieve how many GPUs are found on the system
        uint physicalDeviceCount;
        VK_ASSERT(
            vkEnumeratePhysicalDevices(VK::GetInstance(), &physicalDeviceCount, nullptr),
            "Failed to retrieve available GPUs"
        );

        // Put all found GPUs in an array
        VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[physicalDeviceCount];
        vkEnumeratePhysicalDevices(VK::GetInstance(), &physicalDeviceCount, physicalDevices);

        // TODO: Pick most suitable GPU, not the first one
        ASSERT_ERROR_IF(physicalDeviceCount <= 0, "No GPUs were found on the system");
        ASSERT_ERROR_IF(!PhysicalDeviceSuitable(physicalDevices[0]), "The GPU of your machine is not supported");

        this->physicalDevice = physicalDevices[0];

        ASSERT_SUCCESS_FORMATTED("Vulkan is supported on your system running [{0}] | [Validation: {1} | CPU: {2} | GPU: {3}]", SystemInformation::GetOperatingSystem().name, VALIDATION_ENABLED, SystemInformation::GetCPU().name, physicalDeviceProperties.deviceName);

        // Destroy temporary data
        vkDestroySurfaceKHR(VK::GetInstance(), exampleSurface, nullptr);
        glfwDestroyWindow(glfwWindow);

        delete[] physicalDevices;
    }

    void Device::CreateLogicalDevice()
    {
        // Filter out repeating indices using a set
        const std::set<uint> uniqueQueueFamilies { static_cast<uint>(queueFamilyIndices.graphicsAndComputeFamily), static_cast<uint>(queueFamilyIndices.presentationFamily) };

        // Create an empty list to store create infos
        VkDeviceQueueCreateInfo* queueCreateInfos = new VkDeviceQueueCreateInfo[uniqueQueueFamilies.size()];

        // For each unique family create new create info and add it to the list
        uint i = 0;
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
        logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint>(requiredDeviceExtensions.size());
        logicalDeviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
        logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint>(uniqueQueueFamilies.size());
        logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos;

        // Set up dynamic rendering features
        VkPhysicalDeviceDynamicRenderingFeaturesKHR deviceDynamicRenderingFeatures{};
        deviceDynamicRenderingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
        deviceDynamicRenderingFeatures.dynamicRendering = VK_TRUE;

        #if PLATFORM_APPLE
            // Set apple specific features
            VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilityFeatures{};
            if (portabilitySubsetExtensionEnabled)
            {
                portabilityFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;
                portabilityFeatures.mutableComparisonSamplers = VK_TRUE;
                deviceDynamicRenderingFeatures.pNext = &portabilityFeatures;
            }
        #else
            // Set descriptor indexing features
            VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
            if (GetDescriptorIndexingSupported())
            {
                descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
                descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
                descriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
                descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
                deviceDynamicRenderingFeatures.pNext = &descriptorIndexingFeatures;
            }
        #endif

        // Add chain to device create info
        logicalDeviceCreateInfo.pNext = &deviceDynamicRenderingFeatures;

        // Create logical device
        VK_ASSERT(
            vkCreateDevice(this->physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice),
            "Failed to create logical device"
        );

        // Retrieve queues
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsAndComputeFamily, 0, &graphicsAndComputeQueue);
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentationFamily, 0, &presentationQueue);

        delete[] queueCreateInfos;
    }

    void Device::RetrieveBestProperties()
    {
        this->highestMultisampling = RetrieveMaxSampling();

        this->bestColorImageFormat = GetBestColorBufferFormat(
            { ImageFormat::R8G8B8A8_SRGB, ImageFormat::R8G8B8A8_UNORM },
            ImageTiling::OPTIMAL,
            VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT
        );

        this->bestDepthImageFormat = GetBestDepthBufferFormat(
            { ImageFormat::D32_SFLOAT, ImageFormat::D16_UNORM },
            ImageTiling::OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );

    }

    /* --- GETTER METHODS --- */

    bool Device::PhysicalDeviceSuitable(const VkPhysicalDevice &givenPhysicalDevice)
    {
        // Retrieve GPU's properties
        vkGetPhysicalDeviceProperties(givenPhysicalDevice, &physicalDeviceProperties);

        // Retrieve GPU's memory properties
        vkGetPhysicalDeviceMemoryProperties(givenPhysicalDevice, &physicalDeviceMemoryProperties);

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

        return indicesValid && extensionsSupported && featuresSupported;
    }

    bool Device::DeviceExtensionsSupported(VkPhysicalDevice const &givenPhysicalDevice)
    {
        // Get how many extensions are supported in total
        uint extensionCount;
        vkEnumerateDeviceExtensionProperties(givenPhysicalDevice, nullptr, &extensionCount, nullptr);

        // Create an array to store the supported extensions
        std::vector<VkExtensionProperties> extensionProperties(extensionCount);
        vkEnumerateDeviceExtensionProperties(givenPhysicalDevice, nullptr, &extensionCount, extensionProperties.data());

        #if DEBUG_DEVICE_EXTENSIONS
            for (const auto &extensionProperty : extensionProperties)
            {
                ASSERT_INFO_FORMATTED("Device extension found: {0}", extensionProperty.extensionName);
            }
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
                    if (strcmp(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, extensionProperty.extensionName) == 0)
                    {
                        portabilitySubsetExtensionEnabled = true;
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
                ASSERT_WARNING_FORMATTED("Device extension [{0}] is not supported by your [{1}] GPU", requiredExtension, VK::GetDevice()->GetPhysicalDeviceProperties().deviceName);
            }
        }

        #if __APPLE__
            if (portabilitySubsetExtensionEnabled) requiredDeviceExtensions.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);
        #endif

        if (GetDescriptorIndexingSupported()) requiredDeviceExtensions.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

        return allExtensionsSupported;
    }

    Device::QueueFamilyIndices Device::FindQueueFamilies(const VkPhysicalDevice &givenPhysicalDevice)
    {
        QueueFamilyIndices indices{};

        // Get how many family properties are available
        uint queueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(givenPhysicalDevice, &queueFamilyPropertiesCount, nullptr);

        // Put each of them in an array
        VkQueueFamilyProperties* queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyPropertiesCount];
        vkGetPhysicalDeviceQueueFamilyProperties(givenPhysicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties);

        // Iterate trough each
        for (uint i = queueFamilyPropertiesCount; i--;)
        {
            // Save the current one
            VkQueueFamilyProperties currentQueueProperties = queueFamilyProperties[i];

            // Check if the current queue has a graphics family
            // TODO: Check for asynchronous compute queue
            if ((currentQueueProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT && currentQueueProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
            {
                indices.graphicsAndComputeFamily = i;
            }

            // Check if the current queue supports presentation
            VkBool32 presentationSupported;
            vkGetPhysicalDeviceSurfaceSupportKHR(givenPhysicalDevice, i, exampleSurface, &presentationSupported);

            // Query surface capabilities to save needed data
            VkSurfaceCapabilitiesKHR surfaceCapabilities;
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(givenPhysicalDevice, exampleSurface, &surfaceCapabilities);
            maxConcurrentFrames = glm::max(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount) > 3 ? 3 : glm::max(surfaceCapabilities.minImageCount, surfaceCapabilities.maxImageCount);

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

        delete[] queueFamilyProperties;

        return indices;
    }

    ImageFormat Device::GetBestColorBufferFormat(std::vector<ImageFormat> givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags)
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
        ASSERT_ERROR("No color buffer formats supported");

        return ImageFormat::UNDEFINED;
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

    Sampling Device::RetrieveMaxSampling()
    {
        VkSampleCountFlags countFlags = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;

        if (IS_FLAG_PRESENT(countFlags, Sampling::MSAAx64)) return Sampling::MSAAx64;
        if (IS_FLAG_PRESENT(countFlags, Sampling::MSAAx32)) return Sampling::MSAAx32;
        if (IS_FLAG_PRESENT(countFlags, Sampling::MSAAx16)) return Sampling::MSAAx16;
        if (IS_FLAG_PRESENT(countFlags, Sampling::MSAAx8)) return Sampling::MSAAx8;
        if (IS_FLAG_PRESENT(countFlags, Sampling::MSAAx4)) return Sampling::MSAAx4;
        if (IS_FLAG_PRESENT(countFlags, Sampling::MSAAx2)) return Sampling::MSAAx2;

        return Sampling::MSAAx1;
    }

    /* --- DESTRUCTOR --- */

    void Device::Destroy()
    {
        vkDestroyDevice(logicalDevice, nullptr);
    }
}


