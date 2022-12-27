//
// Created by Nikolay Kanchevski on 19.12.22.
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <glm/mat4x4.hpp>
#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#include "../../Debugger.h"
#include "Abstractions/Device.h"
#include "Abstractions/Descriptors.h"
#include "Abstractions/Texture.h"
#include "Abstractions/Queries.h"

#define VALIDATION_ENABLED DEBUG

using namespace Sierra::Core::Rendering::Vulkan::Abstractions;

namespace Sierra::Core::Rendering::Vulkan
{
    namespace Renderers { class MainVulkanRenderer; }

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
        [[nodiscard]] inline static std::unique_ptr<Device>& GetDevice() { return m_Instance.device; }

        [[nodiscard]] inline static std::shared_ptr<DescriptorSetLayout>& GetDescriptorSetLayout() { return m_Instance.descriptorSetLayout; }

        [[nodiscard]] inline static VkCommandPool GetCommandPool() { return m_Instance.commandPool; }
        [[nodiscard]] inline static std::unique_ptr<QueryPool>& GetQueryPool() { return m_Instance.queryPool; }
        [[nodiscard]] inline static std::unique_ptr<DescriptorPool>& GetDescriptorPool() { return m_Instance.descriptorPool; }

        /* --- DESTRUCTOR --- */
        friend class Renderers::MainVulkanRenderer;
        static void Destroy();

    private:
        VK() = default;
        static VK m_Instance;

        /* --- INSTANCE --- */
        VkInstance instance;
        #if VALIDATION_ENABLED
        VkDebugUtilsMessengerEXT debugMessenger;
        VkDebugUtilsMessengerCreateInfoEXT* debugMessengerCreateInfo;
        void CreateDebugMessenger();
        #endif
        void CreateInstance();

        /* --- DEVICE --- */
        std::unique_ptr<Device> device;
        void CreateDevice();

        /* --- VMA --- */
        VmaAllocator vmaAllocator;
        void CreateVulkanMemoryAllocator();

        /* --- COMMAND POOL --- */
        VkCommandPool commandPool;
        void CreateCommandPool();

        /* --- QUERY POOL --- */
        std::unique_ptr<QueryPool> queryPool;
        void CreateQueryPool();

        /* --- DESCRIPTOR POOL --- */
        std::unique_ptr<DescriptorPool> descriptorPool;
        void CreateDescriptorPool();

        /* --- DEFAULT TEXTURES --- */
        void CreateDefaultTextures();

        /* --- MAIN VULKAN RENDERER --- */
        std::shared_ptr<DescriptorSetLayout> descriptorSetLayout;

    private:
        const std::vector<const char*> requiredInstanceExtensions
        {
            #if VALIDATION_ENABLED
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
            #endif
            "VK_KHR_surface"
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
            uint32_t layerCount = 0;
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
            uint32_t extensionCount;
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

            // Create an array to store the supported extensions
            std::vector<VkExtensionProperties> extensionProperties(extensionCount);
            vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

            // Check if each given extension is in the supported array
            size_t extensionIndex = 0;

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

                    ASSERT_WARNING("Instance extension [" + std::string(requiredExtension) + "] not supported");
                    givenExtensions.erase(givenExtensions.begin() + extensionIndex);
                }

                extensionIndex++;
            }

            return success;
        }
    };

}

#define UNIFORM_BUFFER_BINDING 0
#define STORAGE_BUFFER_BINDING 1

// * ! NOTE: Remember to change these in shaders too ! * //
#define MAX_MESHES 8192                                  // Changed as @kael wouldn't stop bitching about it
#define MAX_TEXTURES MAX_MESHES * TOTAL_TEXTURE_TYPES_COUNT
#define MAX_POINT_LIGHTS 64
#define MAX_DIRECTIONAL_LIGHTS 16