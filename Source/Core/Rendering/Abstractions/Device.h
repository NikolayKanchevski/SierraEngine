//
// Created by Nikolay Kanchevski on 11.12.22.
//

#pragma once

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

    enum class QueueType
    {
        NONE = 0,
        GRAPHICS = VK_QUEUE_GRAPHICS_BIT,
        COMPUTE = VK_QUEUE_COMPUTE_BIT,
        TRANSFER = VK_QUEUE_TRANSFER_BIT
    };

    struct DeviceCreateInfo
    {

    };

    class Device
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit Device([[maybe_unused]] const DeviceCreateInfo &deviceCreateInfo);
        static UniquePtr<Device> Create([[maybe_unused]] const DeviceCreateInfo &createInfo);

        /* --- POLLING METHODS --- */
        void SubmitAndWait(QueueType queueType, const std::vector<ReferenceWrapper<UniquePtr<CommandBuffer>>> &commandBuffersToSubmit, const VkSemaphore* semaphoreToSignal = nullptr);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] inline VkDevice GetLogicalDevice() const { return logicalDevice; }
        [[nodiscard]] inline bool IsExtensionLoaded(const char* extensionName) { auto iterator = loadedExtensions.find(HashType(String(extensionName))); return iterator != loadedExtensions.end() ? iterator->second : false; }

        [[nodiscard]] inline VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() { return physicalDeviceFeatures; }
        [[nodiscard]] inline VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return physicalDeviceProperties; }
        [[nodiscard]] inline VkPhysicalDeviceMemoryProperties& GetPhysicalDeviceMemoryProperties() { return physicalDeviceMemoryProperties; }

        [[nodiscard]] inline uint32 GetMaxConcurrentFramesCount() const { return maxConcurrentFrames; }
        [[nodiscard]] inline ImageFormat GetBestDepthImageFormat() const { return bestDepthImageFormat; }
        [[nodiscard]] inline Sampling GetHighestMultisampling() const { return highestMultisampling; }

        [[nodiscard]] inline uint32 GetGraphicsQueueFamily() const { return queueFamilyIndices.graphicsFamily.value(); }
        [[nodiscard]] inline Optional<uint32> GetComputeQueueFamily() const { return queueFamilyIndices.computeFamily.value(); }
        [[nodiscard]] inline Optional<uint32> GetTransferQueueFamily() const { return queueFamilyIndices.transferFamily.value(); }
        [[nodiscard]] inline uint32 GetPresentationQueueFamily() const { return queueFamilyIndices.presentationFamily.value(); }

        [[nodiscard]] inline VkQueue GetGraphicsQueue() { return graphicsQueue; }
        [[nodiscard]] inline VkQueue GetComputeQueue() { return computeQueue; }
        [[nodiscard]] inline VkQueue GetTransferQueue() { return transferQueue; }
        [[nodiscard]] inline VkQueue GetPresentationQueue() const { return presentationQueue; }

        [[nodiscard]] inline float GetTimestampPeriod() const { return physicalDeviceProperties.limits.timestampPeriod; }

        /* --- SETTER METHODS --- */
        inline void WaitUntilIdle() { vkDeviceWaitIdle(logicalDevice); };

        /* --- DESTRUCTOR --- */
        void Destroy();
        DELETE_COPY(Device);

    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        uint32 maxConcurrentFrames = 0;

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

        VkQueue graphicsQueue;
        VkQueue computeQueue;
        VkQueue transferQueue;
        VkQueue presentationQueue = VK_NULL_HANDLE;

        struct QueueFamilyIndices
        {
            Optional<uint32> graphicsFamily;
            Optional<uint32> computeFamily;
            Optional<uint32> transferFamily;
            Optional<uint32> presentationFamily;
        };
        QueueFamilyIndices queueFamilyIndices;

        ImageFormat bestDepthImageFormat = ImageFormat::UNDEFINED;
        Sampling highestMultisampling = Sampling::MSAAx1;

        void RetrievePhysicalDevice();
        bool PhysicalDeviceSuitable(const VkPhysicalDevice &givenPhysicalDevice, const VkSurfaceKHR exampleSurface);
        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &givenPhysicalDevice, const VkSurfaceKHR exampleSurface);

        template<typename T>
        bool AddExtensionIfSupported(const DeviceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions, T &pNextChain, std::vector<void*> &pointersToDeallocate);
        void CreateLogicalDevice();

        void RetrieveBestProperties();
        ImageFormat GetBestDepthBufferFormat(const std::vector<ImageFormat>& givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags);
        [[nodiscard]] Sampling RetrieveMaxSampling() const;
    };

}