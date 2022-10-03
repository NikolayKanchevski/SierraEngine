//
// Created by Nikolay Kanchevski on 28.09.22.
//

#include "VulkanCore.h"

namespace Sierra::Core::Rendering::Vulkan
{
    Window* VulkanCore::window;
    GLFWwindow* VulkanCore::glfwWindow;
//        static VulkanRenderer vulkanRenderer => window.vulkanRenderer!;

    VkPhysicalDeviceFeatures VulkanCore::physicalDeviceFeatures;
    VkPhysicalDeviceProperties VulkanCore::physicalDeviceProperties;
    VkPhysicalDeviceMemoryProperties VulkanCore::physicalDeviceMemoryProperties;

    VkPhysicalDevice VulkanCore::physicalDevice;
    VkDevice VulkanCore::logicalDevice;

    VkExtent2D VulkanCore::swapchainExtent;
//  float VulkanCore::swapchainAspectRatio => (float) swapchainExtent.width / swapchainExtent.height;

    VkCommandPool VulkanCore::commandPool;

    VkQueue VulkanCore::graphicsQueue;
    uint32_t VulkanCore::graphicsFamilyIndex;
}