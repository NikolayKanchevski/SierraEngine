//
// Created by Nikolay Kanchevski on 5.10.22.
//

#include "VulkanRenderer.h"

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::GetPhysicalDevice()
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
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

        // Loop trough each to see if it supports the program
        bool suitablePhysicalDeviceFound = false;
        for (int i = 0; i < physicalDeviceCount; i++)
        {
            VkPhysicalDevice currentPhysicalDevice = physicalDevices[i];
            if (PhysicalDeviceSuitable(currentPhysicalDevice))
            {
                this->physicalDevice = currentPhysicalDevice;
                suitablePhysicalDeviceFound = true;

                // Assign physical device to Vulkan core
                VulkanCore::SetPhysicalDevice(currentPhysicalDevice);

                // Retrieve the GPU's memory properties
                VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
                vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
                VulkanCore::SetPhysicalDeviceMemoryProperties(deviceMemoryProperties);

                // Show support message
                ASSERT_SUCCESS("Vulkan is supported by your [SOME_PC_MODEL] running SOME_OS [Validation: " + std::to_string(VALIDATION_ENABLED) + " | CPU: SOME_CPU_MODEL | GPU: " + std::string(VulkanCore::GetPhysicalDeviceProperties().deviceName) + "]");

                break;
            }
        }

        ASSERT_ERROR_IF(!suitablePhysicalDeviceFound, "Couldn't find a GPU that supports the program");
    }

    bool VulkanRenderer::PhysicalDeviceSuitable(VkPhysicalDevice &givenPhysicalDevice)
    {
        // Retrieve the GPU's properties
        VkPhysicalDeviceProperties deviceProperties{};
        vkGetPhysicalDeviceProperties(givenPhysicalDevice, &deviceProperties);
        VulkanCore::SetPhysicalDeviceProperties(deviceProperties);

        // Get the features of the given GPU
        VkPhysicalDeviceFeatures deviceFeatures{};
        vkGetPhysicalDeviceFeatures(givenPhysicalDevice, &deviceFeatures);
        VulkanCore::SetPhysicalDeviceFeatures(deviceFeatures);

        // Get the queue indices for it and check if they are valid
        this->queueFamilyIndices = FindQueueFamilies(givenPhysicalDevice);
        bool indicesValid = this->queueFamilyIndices.IsValid();

        // Check if all required extensions are supported
        bool extensionsSupported = DeviceExtensionsSupported(givenPhysicalDevice);

        // Check for required features
        bool featuresSupported = !(this->renderingMode != Fill && !deviceFeatures.fillModeNonSolid);

        // Check if the swapchain type is supported
        SwapchainSupportDetails swapchainSupportDetails = GetSwapchainSupportDetails(givenPhysicalDevice);
        bool swapchainAdequate = !swapchainSupportDetails.formats.empty() && !swapchainSupportDetails.presentModes.empty();

        return indicesValid && extensionsSupported && swapchainAdequate && featuresSupported;
    }

    bool VulkanRenderer::DeviceExtensionsSupported(VkPhysicalDevice &givenPhysicalDevice)
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

        return allExtensionsSupported;
    }

    VulkanRenderer::QueueFamilyIndices VulkanRenderer::FindQueueFamilies(VkPhysicalDevice &givenPhysicalDevice)
    {
        QueueFamilyIndices indices{};

        // Get how many family properties are available
        uint32_t queueFamilyPropertiesCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(givenPhysicalDevice, &queueFamilyPropertiesCount, nullptr);

        // Put each of them in an array
        std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyPropertiesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(givenPhysicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

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
                indices.presentFamily = i;
            }

            // If the indices are already valid there's no need to continue the loop
            if (indices.IsValid())
            {
                break;
            }
        }

        return indices;
    }

}