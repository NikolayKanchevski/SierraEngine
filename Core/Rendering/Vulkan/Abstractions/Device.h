//
// Created by Nikolay Kanchevski on 11.12.22.
//

#pragma once

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>

#include "../VulkanTypes.h"

#define VALIDATION_ENABLED DEBUG

namespace Sierra::Core::Rendering::Vulkan::Abstractions
{

    struct DeviceCreateInfo
    {
        VkPhysicalDeviceFeatures requiredFeatures{};
    };

    class Device
    {
    public:
        /* --- CONSTRUCTORS --- */
        Device(DeviceCreateInfo &deviceCreateInfo);
        static std::unique_ptr<Device> Create(DeviceCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool GetDescriptorIndexingSupported() const
        { return false; } // TODO: BINDLESS
//        #if !__APPLE__
//            { return physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing; };
//        #else
//            { return false; }
//        #endif

        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] VkDevice GetLogicalDevice() const { return logicalDevice; }

        [[nodiscard]] VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const { return physicalDeviceFeatures; }
        [[nodiscard]] VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { return physicalDeviceProperties; }
        [[nodiscard]] VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return physicalDeviceMemoryProperties; }

        [[nodiscard]] ImageFormat GetBestColorImageFormat() const { return bestColorImageFormat; }
        [[nodiscard]] ImageFormat GetBestDepthImageFormat() const { return bestDepthImageFormat; }

        [[nodiscard]] Sampling GetHighestMultisampling() const { return highestMultisampling; }

        [[nodiscard]] uint32_t GetGraphicsQueueFamily() const { return queueFamilyIndices.graphicsFamily; }
        [[nodiscard]] uint32_t GetPresentationQueueFamily() const { return queueFamilyIndices.presentationFamily; }

        [[nodiscard]] VkQueue GetGraphicsQueue() const { return graphicsQueue; }
        [[nodiscard]] VkQueue GetPresentationQueue() const { return presentationQueue; }

        [[nodiscard]] float GetTimestampPeriod() const { return physicalDeviceProperties.limits.timestampPeriod; }

        [[nodiscard]] VkCommandBuffer BeginSingleTimeCommands() const;
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        /* --- SETTER METHODS --- */
        inline void WaitUntilIdle() { vkDeviceWaitIdle(logicalDevice); };

        /* --- DESTRUCTOR --- */
        void Destroy();
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

    private:
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR exampleSurface; // An existing surface is required for device creation so one will be created specifically and destroyed after that

        VkDevice logicalDevice;
        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

        VkQueue graphicsQueue;
        VkQueue presentationQueue;

        struct QueueFamilyIndices
        {
            int graphicsFamily = -1;
            int presentationFamily = -1;

            bool IsValid()
            {
                return graphicsFamily >= 0 && presentationFamily >= 0;
            }
        };

        QueueFamilyIndices queueFamilyIndices;
        VkPhysicalDeviceFeatures* requiredFeatures;

        ImageFormat bestColorImageFormat;
        ImageFormat bestDepthImageFormat;

        Sampling highestMultisampling;

        void RetrievePhysicalDevice();
        void CreateLogicalDevice();

        void RetrieveBestProperties();
        bool DeviceExtensionsSupported(const VkPhysicalDevice &givenPhysicalDevice);

        bool PhysicalDeviceSuitable(const VkPhysicalDevice &givenPhysicalDevice);
        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &givenPhysicalDevice);

        ImageFormat GetBestColorBufferFormat(std::vector<ImageFormat> givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags);
        ImageFormat GetBestDepthBufferFormat(std::vector<ImageFormat> givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags);
        Sampling RetrieveMaxSampling();

        std::vector<const char*> requiredDeviceExtensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

    };

}