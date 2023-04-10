//
// Created by Nikolay Kanchevski on 19.12.22.
//

#pragma once

#include "Abstractions/Device.h"
#include "Abstractions/Descriptors.h"
#include "Abstractions/Texture.h"
#include "Abstractions/Queries.h"

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Core::Rendering::Vulkan
{
    class VK
    {
    public:
        /* --- CONSTRUCTORS --- */
        static void Initialize(VkPhysicalDeviceFeatures givenPhysicalDeviceFeatures);

        /* --- GETTER METHODS --- */
        [[nodiscard]] inline static VkInstance GetInstance() { return m_Instance.instance; }
        [[nodiscard]] inline static VmaAllocator& GetMemoryAllocator() {  return m_Instance.vmaAllocator; }

        [[nodiscard]] inline static VkPhysicalDevice GetPhysicalDevice() { return m_Instance.device->GetPhysicalDevice(); }
        [[nodiscard]] inline static VkDevice GetLogicalDevice() { return m_Instance.device->GetLogicalDevice(); }
        [[nodiscard]] inline static UniquePtr<Device>& GetDevice() { return m_Instance.device; }

        [[nodiscard]] inline static UniquePtr<DescriptorSetLayout>& GetDescriptorSetLayout() { return *m_Instance.globalDescriptorSetLayout; }
        inline static void SetGlobalDescriptorSetLayout(UniquePtr<DescriptorSetLayout>& givenDescriptorSetLayout) { m_Instance.globalDescriptorSetLayout = &givenDescriptorSetLayout; }

        [[nodiscard]] inline static VkCommandPool GetCommandPool() { return m_Instance.commandPool; }
        [[nodiscard]] inline static UniquePtr<QueryPool>& GetQueryPool() { return m_Instance.queryPool; }
        [[nodiscard]] inline static VkDescriptorPool GetImGuiDescriptorPool() { return m_Instance.imGuiDescriptorPool; }

        /* --- DESTRUCTOR --- */
        static void Destroy();

    private:
        VK() = default;
        static VK m_Instance;

        /* --- VOLK --- */
        void InitializeVolk();

        /* --- INSTANCE --- */
        VkInstance instance;
        #if VALIDATION_ENABLED
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo;
        void CreateDebugMessenger();
        #endif
        void CreateInstance();

        /* --- DEVICE --- */
        UniquePtr<Device> device;
        void CreateDevice();

        /* --- VMA --- */
        VmaAllocator vmaAllocator;
        void CreateVulkanMemoryAllocator();

        /* --- COMMAND POOL --- */
        VkCommandPool commandPool;
        void CreateCommandPool();

        /* --- QUERY POOL --- */
        UniquePtr<QueryPool> queryPool;
        void CreateQueryPool();

        VkDescriptorPool imGuiDescriptorPool;
        void CreateImGuiDescriptorPool();

        /* --- DEFAULT TEXTURES --- */
        void CreateDefaultTextures();

        /* --- GLOABAL MESH LAYOUT --- */
        UniquePtr<DescriptorSetLayout> *globalDescriptorSetLayout;

    private:
        const std::vector<const char*> requiredInstanceExtensions
        {
            #if VALIDATION_ENABLED
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            #endif
            VK_KHR_SURFACE_EXTENSION_NAME,
            #if PLATFORM_APPLE
                VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME
            #endif
        };

        std::vector<const char*> requiredDeviceExtensions
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        VkPhysicalDeviceFeatures requiredPhysicalDeviceFeatures;

        #if VALIDATION_ENABLED
            const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

            inline static VkResult CreateDebugUtilsMessengerEXT(VkInstance givenInstance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
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

            inline static void DestroyDebugUtilsMessengerEXT(VkInstance givenInstance, VkDebugUtilsMessengerEXT givenDebugMessenger, const VkAllocationCallbacks* pAllocator)
            {
                auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(givenInstance, "vkDestroyDebugUtilsMessengerEXT");
                if (func != nullptr)
                {
                    func(givenInstance, givenDebugMessenger, pAllocator);
                }
            }

        bool ValidationLayersSupported()
        {
            // Get how many validation layers in total are supported
            uint layerCount = 0;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

            // Create an array and store the supported layers
            std::vector<VkLayerProperties> layerProperties(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

            // Check if the given layers are in the supported array
            for (const auto &requiredLayer : validationLayers)
            {
                bool layerFound = false;
                for (const auto &layerProperty : layerProperties)
                {
                    if (strcmp(requiredLayer, layerProperty.layerName) == 0)
                    {
                        layerFound = true;
                        break;
                    }
                }

                if (!layerFound)
                {
                    return false;
                }
            }

            return true;
        }
        #endif

        inline bool ExtensionsSupported(std::vector<const char*> &givenExtensions)
        {
            // Get how many extensions are supported in total
            uint extensionCount;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            // Create an array to store the supported extensions
            std::vector<VkExtensionProperties> extensionProperties(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

            // Check if each given extension is in the supported array
            uSize extensionIndex = 0;

            bool success = true;
            for (const auto &requiredExtension : givenExtensions)
            {
                bool extensionFound = false;
                for (const auto &extensionProperty : extensionProperties)
                {
                    if (strcmp(requiredExtension, extensionProperty.extensionName) == 0)
                    {
                        extensionFound = true;
                        break;
                    }
                }

                if (!extensionFound)
                {
                    success = false;

                    ASSERT_WARNING_FORMATTED("Instance extension [{0}] not supported", String(requiredExtension));
                    givenExtensions.erase(givenExtensions.begin() + extensionIndex);
                }

                extensionIndex++;
            }

            return success;
        }
    };

}