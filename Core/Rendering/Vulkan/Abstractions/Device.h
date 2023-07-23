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

namespace Sierra::Rendering
{

    struct DeviceExtension
    {
        const char* name;
        std::vector<DeviceExtension> dependencies;
        bool requiredOnlyIfSupported = false;
        void* data = nullptr;
    };

    struct DeviceCreateInfo
    {

    };

    class Device
    {
    public:
        /* --- CONSTRUCTORS --- */
        Device([[maybe_unused]] const DeviceCreateInfo &deviceCreateInfo);
        static UniquePtr<Device> Create([[maybe_unused]] const DeviceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] inline VkDevice GetLogicalDevice() const { return logicalDevice; }
        [[nodiscard]] inline bool IsExtensionLoaded(const char* extensionName) { auto iterator = loadedExtensions.find(HashType(String(extensionName))); return iterator != loadedExtensions.end() ? iterator->second : false; }

        [[nodiscard]] inline VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() { return physicalDeviceFeatures; }
        [[nodiscard]] inline VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return physicalDeviceProperties; }
        [[nodiscard]] inline VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() { return physicalDeviceMemoryProperties; }

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

        [[nodiscard]] UniquePtr<CommandBuffer> BeginSingleTimeCommands() const;
        void EndSingleTimeCommands(const UniquePtr<CommandBuffer> &commandBuffer) const;

        /* --- SETTER METHODS --- */
        inline void WaitUntilIdle() { vkDeviceWaitIdle(logicalDevice); };

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Device);

    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkSurfaceKHR exampleSurface = VK_NULL_HANDLE; // An existing surface is required for device creation so one will be created specifically and destroyed after that
        uint maxConcurrentFrames = 0;

        const std::vector<DeviceExtension> EXTENSIONS_TO_QUERY
        {
            {
                .name = VK_KHR_SWAPCHAIN_EXTENSION_NAME
            },
            {
                .name = VK_KHR_MAINTENANCE3_EXTENSION_NAME
            },
            {
                .name = VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
            },
            {
                .name = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
                .dependencies = {
                    {
                        .name = VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
                        .dependencies = {
                            { .name = VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME },
                        }
                    }
                },
                .data = new VkPhysicalDeviceDynamicRenderingFeaturesKHR {
                    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR,
                    .dynamicRendering = VK_TRUE
                }
            },
            #if PLATFORM_APPLE
                {
                    .name = VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME,
                    .requiredOnlyIfSupported = true,
                    .data = new VkPhysicalDevicePortabilitySubsetFeaturesKHR {
                        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR,
                        .mutableComparisonSamplers = VK_TRUE
                    }
                }
            #endif
        };

        // [Hash of extension name as a string | Wether or not the extension is supported]
        std::unordered_map<Hash, bool> loadedExtensions;

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

            [[nodiscard]] inline bool IsValid() { return graphicsAndComputeFamily.has_value() && presentationFamily.has_value(); }
        };
        QueueFamilyIndices queueFamilyIndices;

        ImageFormat bestDepthImageFormat = ImageFormat::UNDEFINED;
        Sampling highestMultisampling = Sampling::MSAAx1;

        bool PhysicalDeviceSuitable(const VkPhysicalDevice &givenPhysicalDevice);
        void RetrievePhysicalDevice();

        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &givenPhysicalDevice);

        template<typename T>
        bool AddExtensionIfSupported(const DeviceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions, T &pNextChain, std::vector<void*> &pointersToDeallocate);
        void CreateLogicalDevice();

        void RetrieveBestProperties();
        ImageFormat GetBestDepthBufferFormat(const std::vector<ImageFormat>& givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags);
        [[nodiscard]] Sampling RetrieveMaxSampling() const;
    };

}