//
// Created by Nikolay Kanchevski on 6.10.22.
//

#include "VulkanRenderer.h"

#include <set>
#include "../VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateLogicalDevice()
    {
        // Filter out repeating indices using a set
        const std::set<uint32_t> uniqueQueueFamilies { static_cast<uint32_t>(queueFamilyIndices.graphicsFamily), static_cast<uint32_t>(queueFamilyIndices.presentFamily) };

        // Create an empty list to store create infos
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        // For each unique family create new create info and add it to the list
        const float queuePriority = 1.0f;
        for (const auto &queueFamily : uniqueQueueFamilies)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;

            queueCreateInfos.push_back(queueCreateInfo);
        }

        // List required physical device features
        VkPhysicalDeviceFeatures requiredPhysicalDeviceFeatures{};
        if (this->renderingMode != Fill)
            requiredPhysicalDeviceFeatures.fillModeNonSolid = VK_TRUE;

        if (VulkanCore::GetPhysicalDeviceFeatures().samplerAnisotropy)
            requiredPhysicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

        if (this->msaaSamplingEnabled && this->msaaSampleCount != VK_SAMPLE_COUNT_1_BIT)
            requiredPhysicalDeviceFeatures.sampleRateShading = VK_TRUE;

        // Set reset features
        VkPhysicalDeviceHostQueryResetFeatures resetFeatures{};
        resetFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES,
        resetFeatures.hostQueryReset = VK_TRUE;

        VkDeviceCreateInfo logicalDeviceCreateInfo{};

        // Set descriptor indexing features
        #if !__APPLE__
            if (VulkanCore::GetDescriptorIndexingSupported())
            {
                VkPhysicalDeviceDescriptorIndexingFeatures descriptorIndexingFeatures{};
                descriptorIndexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
                descriptorIndexingFeatures.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;
                descriptorIndexingFeatures.runtimeDescriptorArray = VK_TRUE;
                descriptorIndexingFeatures.descriptorBindingVariableDescriptorCount = VK_TRUE;
                descriptorIndexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
                logicalDeviceCreateInfo.pNext = &descriptorIndexingFeatures;
            }

            requiredDeviceExtensions.push_back("VK_EXT_descriptor_indexing");
        #endif

        // Fill in logical device creation info
        logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        logicalDeviceCreateInfo.pEnabledFeatures = &requiredPhysicalDeviceFeatures;
        logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
        logicalDeviceCreateInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();
        logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

        // Create logical device
        VK_ASSERT(
            vkCreateDevice(this->physicalDevice, &logicalDeviceCreateInfo, nullptr, &logicalDevice),
            "Failed to create logical device"
        );


        // Assign the EngineCore's logical device
        VulkanCore::SetLogicalDevice(logicalDevice);

        // Retrieve graphics queue
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.graphicsFamily, 0, &graphicsQueue);

        // Assign the EngineCore's graphicsQueue
        VulkanCore::SetGraphicsQueue(graphicsQueue);

        // Retrieve presentation queue
        vkGetDeviceQueue(logicalDevice, queueFamilyIndices.presentFamily, 0, &presentQueue);
    }

}
