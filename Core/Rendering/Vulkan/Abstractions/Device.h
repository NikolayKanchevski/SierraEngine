//
// Created by Nikolay Kanchevski on 11.12.22.
//

#pragma once

#include "../VulkanTypes.h"
#include "CommandBuffer.h"

#ifdef DEBUG
    #define VALIDATION_ENABLED 1
#else
    #define VALIDATION_ENABLED 0
#endif

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
        explicit Device(DeviceCreateInfo &deviceCreateInfo);
        static UniquePtr<Device> Create(DeviceCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool GetDescriptorIndexingSupported() const
        { return false; } // TODO: BINDLESS
//        #if !__APPLE__
//            { return physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing; };
//        #else
//            { return false; }
//        #endif

        [[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] inline VkDevice GetLogicalDevice() const { return logicalDevice; }

        [[nodiscard]] inline VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const { return physicalDeviceFeatures; }
        [[nodiscard]] inline VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { return physicalDeviceProperties; }
        [[nodiscard]] inline VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return physicalDeviceMemoryProperties; }

        [[nodiscard]] inline uint GetMaxConcurrentFramesCount() const { return maxConcurrentFrames; }

        [[nodiscard]] inline ImageFormat GetBestDepthImageFormat() const { return bestDepthImageFormat; }

        [[nodiscard]] inline Sampling GetHighestMultisampling() const { return highestMultisampling; }

        [[nodiscard]] inline uint GetGraphicsQueueFamily() const { return queueFamilyIndices.graphicsAndComputeFamily.value(); }
        [[nodiscard]] inline uint GetPresentationQueueFamily() const { return queueFamilyIndices.presentationFamily.value(); }
        [[nodiscard]] inline uint GetComputeQueueFamily() const { return queueFamilyIndices.graphicsAndComputeFamily.value(); }

        [[nodiscard]] inline VkQueue GetGraphicsQueue() const { return graphicsAndComputeQueue; }
        [[nodiscard]] inline VkQueue GetPresentationQueue() const { return presentationQueue; }
        [[nodiscard]] inline VkQueue GetComputeQueue() const { return graphicsAndComputeQueue; }

        [[nodiscard]] inline float GetTimestampPeriod() const { return physicalDeviceProperties.limits.timestampPeriod; }
        [[nodiscard]] inline bool IsPortabilitySubsetExtensionEnabled() const
            #if PLATFORM_APPLE
                { return portabilitySubsetExtensionEnabled; }
            #else
                { return false; }
            #endif

        [[nodiscard]] UniquePtr<CommandBuffer> BeginSingleTimeCommands() const;
        void EndSingleTimeCommands(UniquePtr<CommandBuffer> &commandBuffer) const;

        /* --- SETTER METHODS --- */
        inline void WaitUntilIdle() { vkDeviceWaitIdle(logicalDevice); };

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Device);

    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkSurfaceKHR exampleSurface = VK_NULL_HANDLE; // An existing surface is required for device creation so one will be created specifically and destroyed after that
        uint maxConcurrentFrames = 0;

        VkDevice logicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceFeatures physicalDeviceFeatures = {};
        VkPhysicalDeviceProperties physicalDeviceProperties = {};
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties = {};

        VkQueue graphicsAndComputeQueue = VK_NULL_HANDLE;
        VkQueue presentationQueue = VK_NULL_HANDLE;

        struct QueueFamilyIndices
        {
            Optional<uint> graphicsAndComputeFamily;
            Optional<uint> presentationFamily;

            bool IsValid()
            {
                return graphicsAndComputeFamily.has_value() && presentationFamily.has_value();
            }
        };

        QueueFamilyIndices queueFamilyIndices;
        VkPhysicalDeviceFeatures* requiredFeatures;

        ImageFormat bestDepthImageFormat = ImageFormat::UNDEFINED;
        Sampling highestMultisampling = Sampling::MSAAx1;

        void RetrievePhysicalDevice();
        void CreateLogicalDevice();

        void RetrieveBestProperties();
        bool DeviceExtensionsSupported(const VkPhysicalDevice &givenPhysicalDevice);

        bool PhysicalDeviceSuitable(const VkPhysicalDevice &givenPhysicalDevice);
        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &givenPhysicalDevice);

        ImageFormat GetBestDepthBufferFormat(const std::vector<ImageFormat>& givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags);
        [[nodiscard]] Sampling RetrieveMaxSampling() const;

        std::vector<const char*> requiredDeviceExtensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
            VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME
        };

        #if PLATFORM_APPLE
            bool portabilitySubsetExtensionEnabled = false;
        #endif
    };

}