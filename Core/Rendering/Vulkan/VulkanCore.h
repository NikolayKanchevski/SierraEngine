//
// Created by Nikolay Kanchevski on 28.09.22.
//

#pragma once

#include <vulkan/vulkan.h>
#include "../Window.h"

namespace Sierra::Core::Rendering::Vulkan
{
    class VulkanCore
    {
    public:
        static Window* window;
        static GLFWwindow* glfwWindow;
//        static VulkanRenderer vulkanRenderer => window.vulkanRenderer!;

        static VkPhysicalDeviceFeatures physicalDeviceFeatures;
        static VkPhysicalDeviceProperties physicalDeviceProperties;
        static VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;

        static VkPhysicalDevice physicalDevice;
        static VkDevice logicalDevice;

        static VkExtent2D swapchainExtent;
//        static float swapchainAspectRatio => (float) swapchainExtent.width / swapchainExtent.height;

        static VkCommandPool commandPool;

        static VkQueue graphicsQueue;
        static uint32_t graphicsFamilyIndex;
    };
}
