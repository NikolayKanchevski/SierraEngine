//
// Created by Nikolay Kanchevski on 11.12.22.
//

#include "Device.h"

#include "../VK.h"
#include "../../../Engine/Classes/SystemInformation.h"

namespace Sierra::Rendering
{
    /* --- CONSTRUCTORS --- */

    Device::Device(const DeviceCreateInfo &deviceCreateInfo)
    {
        RetrievePhysicalDevice();
        CreateLogicalDevice();
        RetrieveBestProperties();

        // Connect device with Volk
        volkLoadDevice(logicalDevice);
    }

    UniquePtr<Device> Device::Create(const DeviceCreateInfo &createInfo)
    {
        return std::make_unique<Device>(createInfo);
    }

    /* --- GETTER METHODS --- */

    #pragma clang diagnostic push
    #pragma ide diagnostic ignored "readability-convert-member-functions-to-static"
    UniquePtr<CommandBuffer> Device::BeginSingleTimeCommands() const
    {
        UniquePtr<CommandBuffer> commandBuffer = CommandBuffer::Create({ });
        commandBuffer->Begin(CommandBufferUsage::ONE_TIME_SUBMIT);
        return commandBuffer;
    }

    void Device::EndSingleTimeCommands(const UniquePtr<CommandBuffer> &commandBuffer) const
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
    #pragma clang diagnostic pop

    /* --- SETTER METHODS --- */

    void Device::RetrievePhysicalDevice()
    {
        // Set up example surface parameters
        glfwInit();
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

        physicalDevice = physicalDevices[0];

        ASSERT_SUCCESS_FORMATTED("Vulkan is supported on your system [Validation: {0} | GPU: {1}]", VALIDATION_ENABLED, physicalDeviceProperties.deviceName);

        // Destroy temporary data
        vkDestroySurfaceKHR(VK::GetInstance(), exampleSurface, nullptr);
        glfwDestroyWindow(glfwWindow);

        delete[](physicalDevices);
    }

    void Device::CreateLogicalDevice()
    {
        // Filter out repeating indices using a set
        const std::set<uint> uniqueQueueFamilies { static_cast<uint>(queueFamilyIndices.graphicsAndComputeFamily.value()), static_cast<uint>(queueFamilyIndices.presentationFamily.value()) };

        // Create an empty list to store create infos
        VkDeviceQueueCreateInfo* queueCreateInfos = new VkDeviceQueueCreateInfo[uniqueQueueFamilies.size()];

        // For each unique family create new create info and add it to the list
        uint currentQueueIndex = 0;
        const float queuePriority = 1.0f;
        for (const auto &queueFamily : uniqueQueueFamilies)
        {
            queueCreateInfos[currentQueueIndex] = {
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .flags = 0,
                .queueFamilyIndex = queueFamily,
                .queueCount = 1,
                .pQueuePriorities = &queuePriority
            };
            currentQueueIndex++;
        }

        // Get supported extension count
        uint supportedExtensionCount;
        std::vector<VkExtensionProperties> supportedExtensions;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, nullptr);

        // Get supported extensions
        supportedExtensions.resize(supportedExtensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, supportedExtensions.data());

        // Set up device features
        VkPhysicalDeviceFeatures2 deviceFeatures2{};
        deviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        deviceFeatures2.features = physicalDeviceFeatures;

        // Store pointers of each extension's data, as it needs to be deallocated in the end of the function
        std::vector<void*> pointersToDeallocate;
        pointersToDeallocate.resize(EXTENSIONS_TO_QUERY.size());

        // Load queried extensions if supported
        std::vector<const char*> finalExtensions;
        for (const auto &extension : EXTENSIONS_TO_QUERY)
        {
            AddExtensionIfSupported(extension, finalExtensions, supportedExtensions, deviceFeatures2, pointersToDeallocate);
        }

        // Fill in logical device creation info
        VkDeviceCreateInfo logicalDeviceCreateInfo{};
        logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logicalDeviceCreateInfo.pEnabledFeatures = nullptr;
        logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint>(finalExtensions.size());
        logicalDeviceCreateInfo.ppEnabledExtensionNames = finalExtensions.data();
        logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint>(uniqueQueueFamilies.size());
        logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos;
        logicalDeviceCreateInfo.pNext = &deviceFeatures2;

        // Create logical device
        VK_ASSERT(
            vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice),
            "Failed to create logical device"
        );

        // Retrieve queues
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsAndComputeFamily.value(), 0, &graphicsAndComputeQueue);
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentationFamily.value(), 0, &presentationQueue);

        for (const auto &pointer : pointersToDeallocate) free(pointer);
        delete[](queueCreateInfos);
    }

    void Device::RetrieveBestProperties()
    {
        highestMultisampling = RetrieveMaxSampling();
        bestDepthImageFormat = GetBestDepthBufferFormat(
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

        // Check if all support conditions are met
        return queueFamilyIndices.IsValid();
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

        delete[](queueFamilyProperties);
        return indices;
    }

    template<typename T>
    bool Device::AddExtensionIfSupported(const DeviceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions, T &pNextChain, std::vector<void*> &pointersToDeallocate)
    {
        auto &extensionLoadState = loadedExtensions[HashType(String(extension.name))];

        // Check if root extension is found within the supported ones
        bool extensionSupported = false;
        for (const auto &supportedExtension : supportedExtensions)
        {
            if (strcmp(extension.name, supportedExtension.extensionName) == 0)
            {
                if (extension.data != nullptr)
                {
                    VK::PushToPNextChain(&pNextChain, extension.data);
                    pointersToDeallocate.push_back(extension.data);
                }
                extensionSupported = true;
                break;
            }
        }

        // If root extension is not found we do not load it
        if (!extensionSupported)
        {
            if (!extension.requiredOnlyIfSupported) ASSERT_WARNING_FORMATTED("Extension [{0}] requested but not supported! Extension will be discarded, but issues may occur if extensions' support is not checked before their usage", extension.name);
            extensionLoadState = false;
            return extensionLoadState;
        }

        // If found, we then check if all required dependency extensions are supported
        for (const auto &dependencyExtension : extension.dependencies)
        {
            if (!AddExtensionIfSupported(dependencyExtension, extensionList, supportedExtensions, pNextChain, pointersToDeallocate))
            {
                ASSERT_WARNING_FORMATTED("Extension [{0}] requires the support of an unsupported extension [{1}]! Extensions will be discarded, but the application may continue to run, but issues may occur if extensions' support is not checked before their usage", extension.name, dependencyExtension.name);
                extensionLoadState = false;
                return extensionLoadState;
            }
        }

        // Add extension to the list and mark it as fully loaded
        extensionLoadState = true;
        extensionList.push_back(extension.name);
        return extensionLoadState;
    }

    ImageFormat Device::GetBestDepthBufferFormat(const std::vector<ImageFormat>& givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags)
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

    Sampling Device::RetrieveMaxSampling() const
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


