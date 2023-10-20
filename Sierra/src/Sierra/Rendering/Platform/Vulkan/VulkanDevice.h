//
// Created by Nikolay Kanchevski on 10.09.23.
//

#pragma once

#include "VulkanResource.h"
#include "VulkanInstance.h"

#include <vk_mem_alloc.h>

namespace Sierra
{

    struct VulkanDeviceCreateInfo
    {
        const UniquePtr<VulkanInstance> &instance;
    };

    class SIERRA_API VulkanDevice final : public VulkanResource
    {
    public:
        /* --- CONSTRUCTORS --- */
        explicit VulkanDevice(const VulkanDeviceCreateInfo &createInfo);
        static UniquePtr<VulkanDevice> Create(const VulkanDeviceCreateInfo &createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] inline VkDevice GetLogicalDevice() const { return logicalDevice; }
        [[nodiscard]] inline VmaAllocator GetAllocator() const { return vmaAllocator; }

        [[nodiscard]] inline VkPhysicalDeviceProperties& GetPhysicalDeviceProperties() { return physicalDeviceProperties; }
        [[nodiscard]] inline VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures() { return physicalDeviceFeatures; }
        [[nodiscard]] bool IsExtensionLoaded(const String &extensionName) const;

        /* --- DESTRUCTOR --- */
        void Destroy() override;

    private:
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceFeatures physicalDeviceFeatures;

        struct QueueFamilyIndices
        {
            std::optional<uint> transferFamily;
            std::optional<uint> computeFamily;
            std::optional<uint> graphicsFamily;
        };

        struct PhysicalDeviceInfo
        {
            float32 rating = -1.0f;
            VkPhysicalDeviceFeatures features { };
            VkPhysicalDeviceProperties properties { };
            QueueFamilyIndices queueFamilyIndices;
        };

        VkDevice logicalDevice = VK_NULL_HANDLE;
        VmaAllocator vmaAllocator = VK_NULL_HANDLE;

        VkQueue transferQueue = VK_NULL_HANDLE;
        VkQueue computeQueue = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;

        struct DeviceExtension
        {
            String name;
            std::vector<DeviceExtension> dependencies;
            bool requiredOnlyIfSupported = false;
            void* data = nullptr;
        };

        const std::vector<DeviceExtension> DEVICE_EXTENSIONS_TO_QUERY
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
            }
        };
        std::vector<Hash> loadedExtensions;

        bool IsExtensionSupported(const char* extensionName, const std::vector<VkExtensionProperties> &supportedExtensions);
        template<typename T>
        bool AddExtensionIfSupported(const DeviceExtension &extension, std::vector<const char*> &extensionList, const std::vector<VkExtensionProperties> &supportedExtensions, T &pNextChain, std::vector<void*> &extensionDataToFree);
        static PhysicalDeviceInfo GetPhysicalDeviceInfo(VkPhysicalDevice physicalDevice);
        static QueueFamilyIndices GetQueueFamilyIndices(VkPhysicalDevice physicalDevice);

    };

}
