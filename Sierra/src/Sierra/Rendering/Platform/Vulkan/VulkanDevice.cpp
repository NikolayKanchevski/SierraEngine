//
// Created by Nikolay Kanchevski on 10.09.23.
//

#include "VulkanDevice.h"
#if SR_PLATFORM_APPLE
    #include <vulkan/vulkan_beta.h> // For VK_KHR_PORTABILITY_SUBSET_EXTENSION
    #define VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR static_cast<VkStructureType>(1000163000)
#endif

#define VMA_IMPLEMENTATION
#ifdef VMA_STATS_STRING_ENABLED
    #undef VMA_STATS_STRING_ENABLED
    #define VMA_STATS_STRING_ENABLED 0
#endif

#define VMA_STATIC_VULKAN_FUNCTIONS 1
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include <vk_mem_alloc.h>

namespace Sierra
{

    /* --- CONSTRUCTORS --- */

    VulkanDevice::VulkanDevice(const VulkanDeviceCreateInfo &createInfo)
    {
        // Retrieve physical device
        PhysicalDeviceInfo physicalDeviceInfo{};
        {
            // Retrieve number of GPUs found
            uint32 physicalDeviceCount = 0;
            VK_VALIDATE(vkEnumeratePhysicalDevices(createInfo.instance->GetVulkanInstance(), &physicalDeviceCount, nullptr), "Could not enumerate physical devices!");

            SR_ERROR_IF(physicalDeviceCount <= 0, "Could not find any Vulkan supported physical devices!");

            // Retrieve GPUs
            std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
            vkEnumeratePhysicalDevices(createInfo.instance->GetVulkanInstance(), &physicalDeviceCount, physicalDevices.data());

            // Select best GPU to use
            for (uint32 i = 0; i < physicalDeviceCount; i++)
            {
                const PhysicalDeviceInfo currentPhysicalDeviceInfo = GetPhysicalDeviceInfo(physicalDevices[i]);
                if (currentPhysicalDeviceInfo.rating > physicalDeviceInfo.rating)
                {
                    physicalDeviceInfo = currentPhysicalDeviceInfo;
                    physicalDevice = physicalDevices[i];
                }
            }

            // Save physical device info
            physicalDeviceProperties = physicalDeviceInfo.properties;
            physicalDeviceFeatures = physicalDeviceInfo.features;
        }

        // Create logical device from the physical device
        {
            // Filter out duplicated indices
            std::set<uint32> uniqueQueueFamilies;
            if (physicalDeviceInfo.queueFamilyIndices.transferFamily.has_value()) uniqueQueueFamilies.insert(physicalDeviceInfo.queueFamilyIndices.transferFamily.value());
            if (physicalDeviceInfo.queueFamilyIndices.computeFamily.has_value()) uniqueQueueFamilies.insert(physicalDeviceInfo.queueFamilyIndices.computeFamily.value());
            if (physicalDeviceInfo.queueFamilyIndices.graphicsFamily.has_value()) uniqueQueueFamilies.insert(physicalDeviceInfo.queueFamilyIndices.graphicsFamily.value());

            // Set up queue create infos
            std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(uniqueQueueFamilies.size());

            uint32 i = 0;
            const float32 QUEUE_PRIORITY = 1.0f;
            for (const auto &queueFamily : uniqueQueueFamilies)
            {
                auto &queueCreateInfo = queueCreateInfos[i];
                queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount = 1;
                queueCreateInfo.pQueuePriorities = &QUEUE_PRIORITY;
                queueCreateInfo.flags = 0;
                queueCreateInfo.pNext = nullptr;
                i++;
            }

            // Store pointers of each extension's data, as it needs to be deallocated in the end of the function
            std::vector<void*> extensionDataToFree;
            extensionDataToFree.resize(DEVICE_EXTENSIONS_TO_QUERY.size());

            // Retrieve supported extension count
            uint32 supportedExtensionCount = 0;
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, nullptr);

            // Retrieve supported extensions
            std::vector<VkExtensionProperties> supportedExtensions(supportedExtensionCount);
            vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &supportedExtensionCount, supportedExtensions.data());

            // Set up device features
            VkPhysicalDeviceFeatures2 physicalDeviceFeatures2{};
            physicalDeviceFeatures2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;

            // Load queried extensions if supported
            std::vector<const char*> extensionsToLoad;
            #if !SR_PLATFORM_APPLE
                extensionsToLoad.reserve(DEVICE_EXTENSIONS_TO_QUERY.size());
            #else
                extensionsToLoad.reserve(DEVICE_EXTENSIONS_TO_QUERY.size() + 1); // Extra space for VK_KHR_PORTABILITY_SUBSET_EXTENSION
            #endif
            for (const auto &extension : DEVICE_EXTENSIONS_TO_QUERY)
            {
                AddExtensionIfSupported(extension, extensionsToLoad, supportedExtensions, physicalDeviceFeatures2, extensionDataToFree);
            }

            #if SR_PLATFORM_APPLE
                // Set up portability features
                VkPhysicalDevicePortabilitySubsetFeaturesKHR portabilitySubsetFeatures{};
                if (IsExtensionSupported(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME, supportedExtensions))
                {
                    // Add extension to list
                    extensionsToLoad.push_back(VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME);

                    // Configure portability info
                    portabilitySubsetFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;
                    portabilitySubsetFeatures.pNext = nullptr;

                    // Retrieve portability features
                    PushToPNextChain(&physicalDeviceFeatures2, &portabilitySubsetFeatures);
                    vkGetPhysicalDeviceFeatures2(physicalDevice, &physicalDeviceFeatures2);
                }
            #endif
            
            // Set to use already defined features
            physicalDeviceFeatures2.features = physicalDeviceFeatures;

            // Set up device create info
            VkDeviceCreateInfo logicalDeviceCreateInfo{};
            logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.size());
            logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
            logicalDeviceCreateInfo.enabledLayerCount = 0;
            logicalDeviceCreateInfo.ppEnabledLayerNames = nullptr;
            logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32>(extensionsToLoad.size());
            logicalDeviceCreateInfo.ppEnabledExtensionNames = extensionsToLoad.data();
            logicalDeviceCreateInfo.pEnabledFeatures = nullptr;
            logicalDeviceCreateInfo.flags = 0;
            logicalDeviceCreateInfo.pNext = &physicalDeviceFeatures2;

            // Create logical device
            VK_VALIDATE(vkCreateDevice(physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice), "Could not create logical device!");

            // Retrieve queues
            if (physicalDeviceInfo.queueFamilyIndices.transferFamily.has_value()) vkGetDeviceQueue(logicalDevice, physicalDeviceInfo.queueFamilyIndices.transferFamily.value(), 0, &transferQueue);
            if (physicalDeviceInfo.queueFamilyIndices.computeFamily.has_value()) vkGetDeviceQueue(logicalDevice, physicalDeviceInfo.queueFamilyIndices.transferFamily.value(), 0, &computeQueue);
            if (physicalDeviceInfo.queueFamilyIndices.graphicsFamily.has_value()) vkGetDeviceQueue(logicalDevice, physicalDeviceInfo.queueFamilyIndices.transferFamily.value(), 0, &graphicsQueue);

            // Deallocate extension data
            for (const auto &data : extensionDataToFree) std::free(data);

            // Register device for Volk to use
            #if !SR_PLATFORM_APPLE
                volkLoadDevice(logicalDevice);
            #endif
        }

        // Create memory allocator
        {
            // Get pointers to required Vulkan methods
            VmaVulkanFunctions vulkanFunctions{};
            vulkanFunctions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
            vulkanFunctions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
            vulkanFunctions.vkAllocateMemory = vkAllocateMemory;
            vulkanFunctions.vkFreeMemory = vkFreeMemory;
            vulkanFunctions.vkMapMemory = vkMapMemory;
            vulkanFunctions.vkUnmapMemory = vkUnmapMemory;
            vulkanFunctions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
            vulkanFunctions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
            vulkanFunctions.vkBindBufferMemory = vkBindBufferMemory;
            vulkanFunctions.vkBindImageMemory = vkBindImageMemory;
            vulkanFunctions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
            vulkanFunctions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
            vulkanFunctions.vkCreateBuffer = vkCreateBuffer;
            vulkanFunctions.vkDestroyBuffer = vkDestroyBuffer;
            vulkanFunctions.vkCreateImage = vkCreateImage;
            vulkanFunctions.vkDestroyImage = vkDestroyImage;
            vulkanFunctions.vkCmdCopyBuffer = vkCmdCopyBuffer;
            if (createInfo.instance->GetAPIVersion() >= VulkanAPIVersion(1, 1, 0))
            {
                vulkanFunctions.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
                vulkanFunctions.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
                vulkanFunctions.vkBindBufferMemory2KHR = vkBindBufferMemory2KHR;
                vulkanFunctions.vkBindImageMemory2KHR = vkBindImageMemory2KHR;
                vulkanFunctions.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
            }
            if (createInfo.instance->GetAPIVersion() >= VulkanAPIVersion(1, 3, 0))
            {
                vulkanFunctions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
                vulkanFunctions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;
            }

            // Set up allocator creation info
            VmaAllocatorCreateInfo vmaCreteInfo{};
            vmaCreteInfo.instance = createInfo.instance->GetVulkanInstance();
            vmaCreteInfo.physicalDevice = physicalDevice;
            vmaCreteInfo.device = logicalDevice;
            vmaCreteInfo.vulkanApiVersion = createInfo.instance->GetAPIVersion();
            vmaCreteInfo.pVulkanFunctions = &vulkanFunctions;
            vmaCreteInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;

            // Create allocator
            vmaCreateAllocator(&vmaCreteInfo, &vmaAllocator);
        }
    }

    UniquePtr<VulkanDevice> VulkanDevice::Create(const VulkanDeviceCreateInfo &createInfo)
    {
        return std::make_unique<VulkanDevice>(createInfo);
    }

    /* --- GETTER METHODS --- */

    bool VulkanDevice::IsExtensionLoaded(const String &extensionName) const
    {
        return std::find(loadedExtensions.begin(), loadedExtensions.end(), std::hash<String>{}(extensionName)) != loadedExtensions.end();
    }

    /* --- PRIVATE METHODS --- */

    VulkanDevice::PhysicalDeviceInfo VulkanDevice::GetPhysicalDeviceInfo(const VkPhysicalDevice physicalDevice)
    {
        // Retrieve GPU's properties
        VkPhysicalDeviceProperties physicalDeviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        // Retrieve GPU's memory properties
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &physicalDeviceMemoryProperties);

        // Get the features of the given GPU
        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);

        // Get queue families
        QueueFamilyIndices queueFamilyIndices = GetQueueFamilyIndices(physicalDevice);

        // Return device info
        PhysicalDeviceInfo info{};
        info.rating = 1.0f; // TODO: Implement actual rating system
        info.features = physicalDeviceFeatures;
        info.properties = physicalDeviceProperties;
        info.queueFamilyIndices = queueFamilyIndices;
        return info;
    }

    VulkanDevice::QueueFamilyIndices VulkanDevice::GetQueueFamilyIndices(const VkPhysicalDevice physicalDevice)
    {
        // Retrieve queue family count
        uint32 queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

        QueueFamilyIndices indices{};
        if (queueFamilyCount == 0) return indices;

        // Retrieve queue families
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

        // Check which queues are present
        for (uint32 i = 0; i < queueFamilyCount; i++)
        {
            VkQueueFamilyProperties currentQueueFamilyProperties = queueFamilyProperties[i];
            if (currentQueueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
            {
                indices.transferFamily = i;
            }
            if (currentQueueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                indices.computeFamily = i;
            }
            if (currentQueueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                indices.graphicsFamily = i;
            }
        }

        return indices;
    }

    bool VulkanDevice::IsExtensionSupported(const char* extensionName, const std::vector<VkExtensionProperties> &supportedExtensions)
    {
        for (const auto &supportedExtension : supportedExtensions)
        {
            if (strcmp(extensionName, supportedExtension.extensionName) == 0)
            {
                return true;
            }
        }

        return false;
    }

    template<typename T>
    bool VulkanDevice::AddExtensionIfSupported(const DeviceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions, T &pNextChain, std::vector<void*> &extensionDataToFree)
    {
        // Check if root extension is found within the supported ones
        bool extensionSupported = IsExtensionSupported(extension.name.c_str(), supportedExtensions);
        if (extensionSupported && extension.data != nullptr)
        {
            PushToPNextChain(&pNextChain, extension.data);
            extensionDataToFree.push_back(extension.data);
        }

        // If root extension is not found, we do not load it
        if (!extensionSupported)
        {
            if (!extension.requiredOnlyIfSupported) SR_WARNING("Device extension [{0}] requested but not supported! Extension will be discarded, but issues may occur if extensions' support is not checked before their usage!", extension.name);

            // Free all data within the extension tree, as, because the current extension is not supported, its dependencies will never be loaded, and thus creating a memory leak
            std::function<void(const DeviceExtension&)> FreeExtensionTreeLambda = [&FreeExtensionTreeLambda](const DeviceExtension &extension)
            {
                std::free(extension.data);
                for (const auto &dependency : extension.dependencies)
                {
                    FreeExtensionTreeLambda(dependency);
                }
            };
            FreeExtensionTreeLambda(extension);

            return false;
        }

        // If found, we then check if all required dependency extensions are supported
        for (const auto &dependencyExtension : extension.dependencies)
        {
            if (!AddExtensionIfSupported(dependencyExtension, extensionList, supportedExtensions, pNextChain, extensionDataToFree))
            {
                SR_WARNING("Device extension [{0}] requires the support of an unsupported extension [{1}]! Extensions will be discarded and the application may continue to run, but issues may occur if extensions' support is not checked before their usage!", extension.name, dependencyExtension.name);
                return false;
            }
        }

        // Add extension to the list
        loadedExtensions.push_back(std::hash<String>{}(extension.name));
        extensionList.push_back(extension.name.c_str());
        return true;
    }

    /* --- DESTRUCTOR --- */

    void VulkanDevice::Destroy()
    {
        vmaDestroyAllocator(vmaAllocator);
        vkDestroyDevice(logicalDevice, nullptr);
    }

}
