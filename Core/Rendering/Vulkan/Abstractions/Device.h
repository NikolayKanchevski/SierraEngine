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
        struct QueueFamilyIndices
        {
            int graphicsFamily = -1;
            int presentationFamily = -1;

            bool IsValid()
            {
                return graphicsFamily >= 0 && presentationFamily >= 0;
            }
        };

        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR capabilities;
            std::vector<VkSurfaceFormatKHR> formats;
            std::vector<VkPresentModeKHR> presentModes;
        };

    public:

        /* --- CONSTRUCTORS --- */
        Device(DeviceCreateInfo &deviceCreateInfo);
        static std::shared_ptr<Device> Create(DeviceCreateInfo createInfo);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline bool GetDescriptorIndexingSupported() const
        #if !__APPLE__
            { return physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing; };
        #else
            { return false; }
        #endif

        [[nodiscard]] VkInstance GetInstance() const { return instance; }
        [[nodiscard]] VkSurfaceKHR GetSurface() const { return surface; }

        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice() const { return physicalDevice; }
        [[nodiscard]] VkDevice GetLogicalDevice() const { return logicalDevice; }

        [[nodiscard]] VkPhysicalDeviceFeatures GetPhysicalDeviceFeatures() const { return physicalDeviceFeatures; }
        [[nodiscard]] VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const { return physicalDeviceProperties; }
        [[nodiscard]] VkPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties() const { return physicalDeviceMemoryProperties; }

        [[nodiscard]] ImageFormat GetBestSwapchainImageFormat() const { return (ImageFormat) bestSwapchainImageFormat.format; }
        [[nodiscard]] VkColorSpaceKHR GetBestSwapchainColorSpace() const { return bestSwapchainImageFormat.colorSpace; }
        [[nodiscard]] ImageFormat GetBestDepthImageFormat() const { return bestDepthImageFormat; }

        [[nodiscard]] VkSurfaceCapabilitiesKHR GetSwapchainCapabilites()
        {
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &swapchainSupportDetails.capabilities);
            return swapchainSupportDetails.capabilities;
        }
        [[nodiscard]] VkPresentModeKHR GetBestPresentationMode() const { return bestPresentationMode; }

        [[nodiscard]] Multisampling GetHighestMultisampling() const { return highestMultisampling; }

        [[nodiscard]] uint32_t GetGraphicsQueueFamily() const { return queueFamilyIndices.graphicsFamily; }
        [[nodiscard]] uint32_t GetPresentationQueueFamily() const { return queueFamilyIndices.presentationFamily; }

        [[nodiscard]] VkQueue GetGraphicsQueue() const { return graphicsQueue; }
        [[nodiscard]] VkQueue GetPresentationQueue() const { return presentationQueue; }

        [[nodiscard]] VkCommandPool GetCommandPool() const { return commandPool; }

        [[nodiscard]] uint32_t FindMemoryTypeIndex(uint32_t typeFilter, MemoryFlags givenMemoryFlags) const;
        [[nodiscard]] VkCommandBuffer BeginSingleTimeCommands() const;
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

        [[nodiscard]] VkShaderModule CreateShaderModule(const std::string &fileName);

        /* --- DESTRUCTOR --- */
        void Destroy();
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

    private:
        inline static const std::vector<const char*> requiredInstanceExtensions
        {
            #if VALIDATION_ENABLED
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            #endif
        };

        std::vector<const char*> requiredDeviceExtensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        #if VALIDATION_ENABLED
        inline static const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

        inline VkResult CreateDebugUtilsMessengerEXT(VkInstance givenInstance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
        {
            auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(givenInstance, "vkCreateDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                return func(givenInstance, pCreateInfo, pAllocator, pDebugMessenger);
            }
            else
            {
                return VK_ERROR_EXTENSION_NOT_PRESENT;
            }
        }

        inline void DestroyDebugUtilsMessengerEXT(VkInstance givenInstance, VkDebugUtilsMessengerEXT givenDebugMessenger, const VkAllocationCallbacks* pAllocator)
        {
            auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(givenInstance, "vkDestroyDebugUtilsMessengerEXT");
            if (func != nullptr)
            {
                func(givenInstance, givenDebugMessenger, pAllocator);
            }
        }
        #endif

    private:
        VkInstance instance;
        VkSurfaceKHR surface;

        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;

        VkPhysicalDeviceFeatures physicalDeviceFeatures;
        VkPhysicalDeviceProperties physicalDeviceProperties;
        VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

        VkQueue graphicsQueue;
        VkQueue presentationQueue;

        VkCommandPool commandPool;

        QueueFamilyIndices queueFamilyIndices;
        SwapchainSupportDetails swapchainSupportDetails;

        VkPhysicalDeviceFeatures* requiredFeatures;

        VkSurfaceFormatKHR bestSwapchainImageFormat;
        ImageFormat bestDepthImageFormat;
        VkPresentModeKHR bestPresentationMode;
        Multisampling highestMultisampling;

        /* --- SETTER METHODS --- */
        void CreateInstance();

        #if VALIDATION_ENABLED
        void CreateDebugMessenger();
        #endif

        void CreateSurface();
        void RetrievePhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();
        void RetrieveBestProperties();

        /* --- GETTER METHODS --- */
        #if VALIDATION_ENABLED
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo;
        bool ValidationLayersSupported();
        #endif

        bool ExtensionsSupported(std::vector<const char*> &givenExtensions);
        bool DeviceExtensionsSupported(const VkPhysicalDevice &givenPhysicalDevice);

        bool PhysicalDeviceSuitable(const VkPhysicalDevice &givenPhysicalDevice);

        QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice &givenPhysicalDevice);
        SwapchainSupportDetails GetSwapchainSupportDetails(const VkPhysicalDevice &givenPhysicalDevice);

        VkSurfaceFormatKHR ChooseBestSwapchainFormat(std::vector<VkSurfaceFormatKHR> &givenFormats);
        ImageFormat GetBestDepthBufferFormat(std::vector<ImageFormat> givenFormats, ImageTiling imageTiling, VkFormatFeatureFlagBits formatFeatureFlags);
        VkPresentModeKHR ChooseSwapchainPresentMode(std::vector<VkPresentModeKHR> &givenPresentModes);
        Multisampling GetHighestSupportedSampling();
    };

}