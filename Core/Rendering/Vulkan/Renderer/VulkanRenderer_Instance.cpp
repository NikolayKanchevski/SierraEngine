//
// Created by Niki on 10/4/2022.
//

#include <iostream>
#include "VulkanRenderer.h"
#include "../../../../Engine/Classes/Version.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using namespace Sierra::Engine::Classes;

namespace Sierra::Core::Rendering::Vulkan
{

    void VulkanRenderer::CreateInstance()
    {
        // Create application information
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Sierra Engine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.apiVersion = VK_API_VERSION_1_1;

        // Get GLFW extensions
        uint32_t glfwExtensionCount;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        for (auto &requiredExtension : requiredInstanceExtensions)
        {
            extensions.push_back(requiredExtension);
        }

        // Check all extensions
        CheckExtensionsSupport(extensions);

        // Set up instance creation info
        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

        // If validation is enabled check validation layers support and bind them to instance
        if (VALIDATION_ENABLED)
        {
            if (!ValidationLayersSupported())
            {
                VulkanDebugger::ThrowWarning("Validation layers requested, but not available. Returning");
            }
            else
            {
                instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
                instanceCreateInfo.ppEnabledLayerNames = validationLayers.data();

                VkDebugUtilsMessengerCreateInfoEXT validationMessengerCreateInfo{};
                GetValidationMessengerCreateInfo(validationMessengerCreateInfo);
                instanceCreateInfo.pNext =  (VkDebugUtilsMessengerCreateInfoEXT*) &validationMessengerCreateInfo;
            }
        }

        // Create instance
        VulkanDebugger::CheckResults(
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance),
            "Could not create Vulkan instance"
        );
    }

    void VulkanRenderer::CheckExtensionsSupport(std::vector<const char *> &givenExtensions)
    {
        // Get how many extensions are supported in total
        uint extensionCount;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        // Create an array to store the supported extensions
        std::vector<VkExtensionProperties> extensionProperties(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensionProperties.data());

        // Check if each given extension is in the supported array
        size_t extensionIndex = 0;
        for (auto &requiredExtension : givenExtensions)
        {
            bool extensionFound = false;
            for (auto &extensionProperty : extensionProperties)
            {
                if (strcmp(requiredExtension, extensionProperty.extensionName) == 0)
                {
                    extensionFound = true;
                    break;
                }
            }

            if (!extensionFound)
            {
                VulkanDebugger::ThrowWarning("Instance extension [" + std::string(requiredExtension) + "] not supported");
                givenExtensions.erase(givenExtensions.begin() + extensionIndex);
            }

            extensionIndex++;
        }
    }

    bool VulkanRenderer::ValidationLayersSupported()
    {
        // Get how many validation layers in total are supported
        uint layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        // Create an array and store the supported layers
        std::vector<VkLayerProperties> layerProperties(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

        // Check if the given layers are in the supported array
        for (auto &requiredLayer : validationLayers)
        {
            bool layerFound = false;
            for (auto &layerProperty : layerProperties)
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

}