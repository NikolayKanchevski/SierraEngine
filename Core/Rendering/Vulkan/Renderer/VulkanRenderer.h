//
// Created by Nikolay Kanchevski on 4.10.22.
//

#pragma once

#include <vector>
#include "../VulkanDebugger.h"

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanRenderer
    {
    public:
        /* --- CONSTRUCTORS --- */
        VulkanRenderer() = default;
        void Start();
        void Update();

        /* --- DESTRUCTOR --- */
        ~VulkanRenderer();
        VulkanRenderer(const VulkanRenderer &) = delete;
        VulkanRenderer &operator=(const VulkanRenderer &) = delete;
    private:

        /* --- Instance --- */
        const std::vector<const char*> requiredInstanceExtensions
        {
            #if DEBUG
                VK_EXT_DEBUG_UTILS_EXTENSION_NAME
            #endif
        };

        const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };

        VkInstance instance;

        void CreateInstance();
        void CheckExtensionsSupport(std::vector<const char *> &givenExtensions);
        bool ValidationLayersSupported();

        /* --- VALIDATION --- */
        #if DEBUG
            bool VALIDATION_ENABLED = true;
        #else
            bool VALIDATION_ENABLED = false;
        #endif

        VkDebugUtilsMessengerEXT validationMessenger;

        void CreateValidationMessenger();
        void GetValidationMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        VkResult CreateDebugUtilsMessengerEXT(VkInstance givenInstance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
        void DestroyDebugUtilsMessengerEXT(VkInstance givenInstance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    };

}