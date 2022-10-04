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
        VkApplicationInfo applicationInfo{};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.pApplicationName = "Sierra Engine";
        applicationInfo.applicationVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.pEngineName = "No Engine";
        applicationInfo.engineVersion = VK_MAKE_VERSION(Version::MAJOR, Version::MINOR, Version::PATCH);
        applicationInfo.apiVersion = VK_API_VERSION_1_2;

        uint32_t glfwExtensionCount;
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
        for (auto &requiredExtension : requiredInstanceExtensions)
        {
            extensions.push_back(requiredExtension);
        }

        VkInstanceCreateInfo instanceCreateInfo{};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.pApplicationInfo = &applicationInfo;
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
        instanceCreateInfo.enabledLayerCount = 0;
        instanceCreateInfo.ppEnabledLayerNames = nullptr;

        VulkanDebugger::CheckResults(
            vkCreateInstance(&instanceCreateInfo, nullptr, &instance),
            "Could not create Vulkan instance"
        );
    }

}